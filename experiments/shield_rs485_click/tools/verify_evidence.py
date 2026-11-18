"""Offline independent comparison of wire logs and frozen MCU RAM, version 1.

No serial/debugger access and no mutation of evidence. Prints a JSON verdict.
"""
import json
from pathlib import Path
import struct
import sys


def crc(data):
    value = 0xFFFF
    for byte in data:
        value ^= byte
        for _ in range(8):
            value = (value >> 1) ^ (0xA001 if value & 1 else 0)
    return value


def check(directory):
    names = ("version stage mode error uid0 uid1 uid2 core_hz credits "
             "rx_count trace_count de_count frames tx_count").split()
    request_prefix = b""
    phases = {}
    for phase, count, tx_count in (("RX", 1, 0), ("RAW", 8, 8), ("MODBUS", 8, 16)):
        rows = [json.loads(line) for line in
                (directory / f"native-{phase}-01.jsonl").read_text().splitlines()]
        requests = [bytes.fromhex(row["request_hex"]) for row in rows if row["kind"] == "request"]
        replies = [bytes.fromhex(row["response_hex"]) for row in rows if row["kind"] == "response"]
        assert len(requests) == len(replies) == count
        assert rows[-1]["kind"] == "closed"
        assert rows[-2]["kind"] == "summary" and rows[-2]["host_passed"]
        assert not any(row["kind"] == "failure" for row in rows)
        frozen = (directory / f"001-{phase}-01-probe.bin").read_bytes()
        assert len(frozen) == 27704  # C struct v1; fixed u32 and byte arrays, no pointers
        state = dict(zip(names, struct.unpack_from("<14I", frozen)))
        assert state["version"] == state["stage"] == 1 and state["error"] == 0
        assert [state[f"uid{i}"] for i in range(3)] == [0x004A003D, 0x38425018, 0x20333157]
        assert state["core_hz"] == 160000000 and state["tx_count"] == tx_count
        request_prefix += b"".join(requests)
        assert frozen[56:56+state["rx_count"]] == request_prefix
        for sequence, (request, reply) in enumerate(zip(requests, replies)):
            assert crc(request) == 0
            if phase == "RX":
                assert reply == b""
            else:
                assert crc(reply) == 0 and request != reply
                if phase == "RAW":
                    assert len(reply) == 26 and reply[:4] == b"T18!"
                    assert reply[4:12] == request[4:12]
                    assert struct.unpack_from("<I", request, 8)[0] == sequence
                    assert struct.unpack_from("<3I", reply, 12) == (0x004A003D, 0x38425018, 0x20333157)
                else:
                    assert request == bytes.fromhex("01 03 03 E8 00 01 04 7A")
                    assert reply == bytes.fromhex("01 03 02 00 00 B8 44")
        trace = [struct.unpack_from("<3I", frozen, 1080+12*i) for i in range(state["trace_count"])]
        assert state["trace_count"] < 2048 and state["rx_count"] < 1024
        assert not any(event[1] == 4 for event in trace)
        assert bytes(event[2] for event in trace if event[1] == 1) == request_prefix
        phases[phase] = {"verdict": "PASS_PHYSICAL_BOUNDED", "requests": count,
                         "RX_host_equals_RAM": True, "state": state}

    # Final snapshot contains all 17 received frames and all 16 real transmissions.
    assert state["de_count"] == 34 and state["trace_count"] == 224 and state["rx_count"] == 190
    de = [struct.unpack_from("<4I", frozen, 25656+16*i) for i in range(state["de_count"])]
    assert [edge[1] for edge in de[2:]] == [1, 0] * 16
    assert all(bool(edge[2] & 16) == bool(edge[1]) == bool(edge[3] & 16) for edge in de[2:])
    groups, group = [], []
    for event in trace:
        group.append(event)
        if event[1] == 3:
            groups.append(group)
            group = []
    assert len(groups) == 17 and not group
    timings = []
    for index, group in enumerate(groups[1:]):
        last_rx = [event for event in group if event[1] == 1][-1][0]
        high, low = de[2+2*index:4+2*index]
        delay = ((high[0] - last_rx) & 0xFFFFFFFF) / 160
        width = ((low[0] - high[0]) & 0xFFFFFFFF) / 160
        # Coherence bounds, not a metrological certification or SI tolerance claim.
        assert 1700 < delay < 2000
        assert (2400 < width < 2700) if index < 8 else (600 < width < 900)
        timings.append({"phase": "RAW" if index < 8 else "MODBUS", "sequence": index % 8,
                        "RX_last_to_DE_high_nominal_us": delay,
                        "DE_high_nominal_us": width})
    return {"verdict": "PASS_RX_TX_MODBUS_PHYSICAL_BOUNDED", "phases": phases,
            "DE_pairs_IDR_ODR_consistent": 16, "timings_nominal_MCU_clock": timings,
            "limitations": ["MCU DWT instrumentation, not an oscilloscope at Click",
                            "No calibrated SI timing, rail ripple or HV isolation certification",
                            "B1 UNKNOWN0 is diagnostic, no sensor/media or full shield qualification"]}


if __name__ == "__main__":
    print(json.dumps(check(Path(sys.argv[1])), indent=2))
