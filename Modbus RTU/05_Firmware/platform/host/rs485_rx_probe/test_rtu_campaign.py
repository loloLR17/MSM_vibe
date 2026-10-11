"""Tests isolated in software: no port, no pySerial, no MCU."""
import contextlib
import io
import tempfile
import unittest
from types import SimpleNamespace
from pathlib import Path
from unittest.mock import patch

import rtu_campaign as campaign


class CampaignTests(unittest.TestCase):
    def run_mock(self, responses=None, **kwargs):
        clock = campaign.FakeClock()
        port = campaign.MockPort(clock, responses, **kwargs)
        rows = []
        result = campaign.run_campaign(port, rows.append, clock, True)
        self.assertTrue(port.closed)
        return result, port, rows, clock

    def test_golden_crc_and_fixed_plan(self):
        self.assertEqual(campaign.request().hex(), "010303e80001047a")
        self.assertEqual(campaign.crc(campaign.EXPECTED), 0)
        plan = campaign.plan()
        self.assertEqual(len(plan), 12)
        self.assertNotEqual(campaign.crc(plan[4][1]), 0)
        self.assertEqual(plan[7][1][0], 2)
        self.assertEqual(campaign.crc(plan[7][1]), 0)

    def test_campaign_recovers_after_each_negative(self):
        passed, port, rows, clock = self.run_mock()
        self.assertTrue(passed)
        self.assertEqual(len(port.writes), 12)
        results = [row for row in rows if row["event"] == "result"]
        self.assertEqual(sum(r["verdict"] == "exact_response" for r in results), 10)
        self.assertEqual(sum(r["verdict"] == "expected_silence" for r in results), 2)
        self.assertLess(clock.now, 20)
        self.assertTrue(all(r["simulated"] and r["utc"] for r in rows))
        self.assertEqual(results[0]["raw_hex"], "01 03 02 00 00 b8 44")

    def test_faults_stop_without_retry(self):
        faults = [b"", campaign.request(), campaign.request() + campaign.EXPECTED,
                  campaign.EXPECTED + b"\x00", campaign.EXPECTED[:-1],
                  bytes.fromhex("01 83 04 40 F3"), b"x" * 257]
        for raw in faults:
            with self.subTest(raw=raw):
                passed, port, rows, _ = self.run_mock([raw])
                self.assertFalse(passed)
                self.assertEqual(len(port.writes), 1)
                self.assertEqual(rows[-1]["event"], "stop")

    def test_negative_unexpected_reply_stops(self):
        responses = [campaign.EXPECTED] * 5
        passed, port, _, _ = self.run_mock(responses)
        self.assertFalse(passed)
        self.assertEqual(len(port.writes), 5)

    def test_stale_rx_prevents_first_write(self):
        passed, port, rows, _ = self.run_mock(stale=b"old")
        self.assertFalse(passed)
        self.assertEqual(port.writes, [])
        self.assertEqual(rows[-1]["reason"], "preexisting_or_late_rx")

    def test_partial_write_never_retried(self):
        passed, port, rows, _ = self.run_mock(short_write=True)
        self.assertFalse(passed)
        self.assertEqual(len(port.writes), 1)
        self.assertIn("partial write", rows[-1]["message"])

    def test_logging_failure_closes_without_transmission(self):
        clock = campaign.FakeClock()
        port = campaign.MockPort(clock)
        def broken_log(row):
            raise OSError("disk full")
        with self.assertRaises(OSError):
            campaign.run_campaign(port, broken_log, clock, True)
        self.assertTrue(port.closed)
        self.assertEqual(port.writes, [])

    def test_delayed_bytes_stop_next_write(self):
        clock = campaign.FakeClock()
        class LatePort(campaign.MockPort):
            @property
            def in_waiting(self):
                return 1 if self.clock.now > 1.15 else super().in_waiting
        port = LatePort(clock)
        rows = []
        self.assertFalse(campaign.run_campaign(port, rows.append, clock, True))
        self.assertEqual(len(port.writes), 1)
        self.assertEqual(rows[-1]["reason"], "preexisting_or_late_rx")

    def test_offline_and_mock_cannot_open_real_port(self):
        with patch.object(campaign, "open_hp_port", side_effect=AssertionError("port opened")):
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(campaign.main([]), 0)
            with tempfile.TemporaryDirectory() as directory:
                log = str(Path(directory) / "mock.jsonl")
                self.assertEqual(campaign.main(["--mock", "--log", log]), 0)
                with self.assertRaises(FileExistsError):
                    campaign.main(["--mock", "--log", log])

    def test_serial_adapter_configuration_without_a_device(self):
        class Port:
            def __init__(self, **kwargs):
                self.options = kwargs
                self.opened = False
            def open(self):
                self.opened = True
                self.settings_at_open = (self.port, self.dtr, self.rts)
            def close(self):
                self.opened = False
        library = SimpleNamespace(Serial=Port, EIGHTBITS=8, PARITY_EVEN="E", STOPBITS_ONE=1)
        with patch.dict("sys.modules", serial=library):
            port = campaign.open_hp_port()
        self.assertIsNone(port.options["port"])
        self.assertEqual(port.settings_at_open, ("COM7", False, False))
        self.assertEqual(port.options["timeout"], 0)
        self.assertEqual(port.options["write_timeout"], 1)
        self.assertFalse(port.options["rtscts"])
        port.close()


if __name__ == "__main__":
    unittest.main()
