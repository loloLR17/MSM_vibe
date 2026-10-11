"""M013-G preparation. Offline by default; no automatic retry or echo stripping."""
import argparse
import datetime
import json
from pathlib import Path
import sys
import time

WINDOW = 1.0
GAP = 0.1
MAX_BYTES = 256
EXPECTED = bytes.fromhex("01 03 02 00 00 B8 44")


def crc(data):
    value = 0xFFFF
    for byte in data:
        value ^= byte
        for _ in range(8):
            value = (value >> 1) ^ (0xA001 if value & 1 else 0)
    return value


def request(unit=1):
    body = bytes([unit, 3, 3, 232, 0, 1])
    return body + crc(body).to_bytes(2, "little")


def plan():
    bad = bytearray(request())
    bad[-1] ^= 1
    nominal = ("nominal", request(), EXPECTED)
    return ([nominal] * 4 + [("bad_crc", bytes(bad), b"")] +
            [nominal] * 2 + [("foreign_unit_2", request(2), b"")] +
            [nominal] * 4)


def classify(raw, sent, expected):
    # Any echo (including echo + response), extra bytes or exception is a stop.
    if raw and raw.startswith(sent):
        return "echo_or_ambiguous", False
    if not expected:
        return ("expected_silence", True) if not raw else ("unexpected_rx", False)
    if not raw:
        return "timeout", False
    if len(raw) < 4 or crc(raw) != 0:
        return "invalid_crc_or_partial", False
    return ("exact_response", True) if raw == expected else ("unexpected_response", False)


def run_campaign(port, emit, clock=time, simulated=False):
    """Bounded shared engine; injected clock/transport allow entirely off-bus tests."""
    started = clock.monotonic()

    def log(event, **fields):
        emit(dict(event=event, utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  elapsed_s=clock.monotonic() - started, simulated=simulated, **fields))

    try:
        log("start", requests_max=12, port="MOCK" if simulated else "COM7",
            baud=115200, bits=8, parity="E", stop=1, window_s=WINDOW, gap_s=GAP)
        clock.sleep(GAP)
        for index, (case, sent, expected) in enumerate(plan(), 1):
            if clock.monotonic() - started >= 20:
                raise RuntimeError("campaign deadline exceeded")
            if port.in_waiting:
                # Preserve queued evidence; never silently flush stale/late bytes.
                stale = port.read(MAX_BYTES)
                log("stop", index=index, reason="preexisting_or_late_rx", raw_hex=stale.hex(" "))
                return False
            log("write_intent", index=index, case=case, raw_hex=sent.hex(" "))
            began = clock.monotonic()
            count = port.write(sent)
            log("write_result", index=index, written=count)
            if count != len(sent):
                raise RuntimeError("partial write; never retry")
            raw = bytearray()
            first_rx = None
            while clock.monotonic() - began < WINDOW:
                chunk = port.read(MAX_BYTES + 1 - len(raw))
                if chunk:
                    if first_rx is None:
                        first_rx = clock.monotonic() - began
                    raw.extend(chunk)
                    log("rx_chunk", index=index, raw_hex=chunk.hex(" "))
                    if len(raw) > MAX_BYTES:
                        raise RuntimeError("RX byte limit exceeded")
                clock.sleep(0.005)
            verdict, passed = classify(bytes(raw), sent, expected)
            log("result", index=index, case=case, raw_hex=raw.hex(" "),
                crc_residue=crc(raw) if raw else None, verdict=verdict, passed=passed,
                first_rx_host_s=first_rx, observation_s=clock.monotonic() - began)
            if not passed:
                log("stop", index=index, reason=verdict)
                return False
            clock.sleep(GAP)
        # A response arriving after the final observation window is also a failure.
        if port.in_waiting:
            log("stop", reason="late_final_rx", raw_hex=port.read(MAX_BYTES).hex(" "))
            return False
        log("complete", passed=True, writes=12, nominal=10, negative=2)
        return True
    except Exception as error:
        log("stop", reason=type(error).__name__, message=str(error))
        return False
    finally:
        port.close()


class FakeClock:
    def __init__(self):
        self.now = 0.0

    def monotonic(self):
        return self.now

    def sleep(self, duration):
        self.now += duration


class MockPort:
    """Software-only transport, deliberately never imports a serial library."""
    def __init__(self, clock, responses=None, stale=b"", short_write=False):
        self.clock = clock
        self.responses = list(responses) if responses is not None else [x[2] for x in plan()]
        self.pending = stale
        self.writes = []
        self.closed = False
        self.short_write = short_write

    @property
    def in_waiting(self):
        return len(self.pending)

    def write(self, data):
        self.writes.append(data)
        self.pending = self.responses.pop(0)
        return len(data) - 1 if self.short_write else len(data)

    def read(self, size):
        # Split the response to exercise accumulation across reads.
        chunk = self.pending[:min(size, 2)]
        self.pending = self.pending[len(chunk):]
        return chunk

    def close(self):
        self.closed = True


def open_hp_port():
    # Lazy import: offline/mock operation has no pySerial dependency or port access.
    import serial
    port = serial.Serial(port=None, baudrate=115200, bytesize=serial.EIGHTBITS,
                         parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_ONE,
                         timeout=0, write_timeout=1, xonxoff=False, rtscts=False, dsrdtr=False)
    port.dtr = False
    port.rts = False
    port.port = "COM7"
    try:
        port.open()
    except Exception:
        port.close()
        raise
    return port


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--offline", action="store_true")
    mode.add_argument("--mock", action="store_true")
    mode.add_argument("--transmit", action="store_true")
    parser.add_argument("--authorized-mission", default="")
    parser.add_argument("--log", type=Path)
    args = parser.parse_args(argv)
    if not args.mock and not args.transmit:
        print(json.dumps(dict(mode="OFFLINE", port_opened=False,
                              plan=[dict(case=c, request=s.hex(" "), expected=e.hex(" "))
                                    for c, s, e in plan()]), indent=2))
        return 0
    if args.transmit and (sys.platform != "win32" or not args.authorized_mission.strip()):
        parser.error("physical mode requires Windows HP and an explicitly authorized future mission")
    if args.log is None:
        parser.error("--log is required; existing logs are never overwritten")
    # Create log before opening a port: a disk/path error must prevent any transmission.
    with args.log.open("x", encoding="utf-8") as output:
        def emit(row):
            row["authorized_mission"] = args.authorized_mission if args.transmit else "MOCK_ONLY"
            output.write(json.dumps(row) + "\n")
            output.flush()
        clock = FakeClock() if args.mock else time
        try:
            port = MockPort(clock) if args.mock else open_hp_port()
        except Exception as error:
            emit(dict(event="open_failed", simulated=False, message=str(error),
                      utc=datetime.datetime.now(datetime.timezone.utc).isoformat()))
            return 1
        return 0 if run_campaign(port, emit, clock, args.mock) else 1


if __name__ == "__main__":
    raise SystemExit(main())
