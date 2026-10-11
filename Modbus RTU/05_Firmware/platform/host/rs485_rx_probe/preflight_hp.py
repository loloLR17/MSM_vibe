"""Nonintrusive M013-H preflight: never opens a serial device; never grants GO."""
import contextlib
import hashlib
import importlib
import io
import json
from pathlib import Path
import platform
import sys
import tempfile
from unittest.mock import patch

import rtu_campaign as campaign


def inspect_environment():
    result = dict(mode="PREFLIGHT_ONLY", port_opened=False, transmission=False,
                  campaign_go=False, platform=sys.platform, python=platform.python_version(),
                  python_executable=sys.executable, serial_version=None, errors=[])
    if sys.platform != "win32":
        result["errors"].append("Windows HP required; this host cannot establish HP readiness")
    if sys.version_info < (3, 9):
        result["errors"].append("Python >=3.9 required")
    try:
        serial = importlib.import_module("serial")
        result["serial_version"] = getattr(serial, "__version__", None)
        result["serial_file"] = getattr(serial, "__file__", None)
        if result["serial_version"] != "3.5" or not callable(getattr(serial, "Serial", None)):
            result["errors"].append("pySerial 3.5 with Serial API required")
    except ImportError as error:
        result["errors"].append(str(error))
    # Exercise the actual CLI paths with its sole hardware-opening function forbidden.
    # Importing pySerial above does not instantiate Serial or enumerate/open devices.
    with patch.object(campaign, "open_hp_port", side_effect=AssertionError("physical open forbidden")):
        output = io.StringIO()
        with contextlib.redirect_stdout(output):
            offline_rc = campaign.main(["--offline"])
        offline = json.loads(output.getvalue())
        with tempfile.TemporaryDirectory(prefix="tr2-offbus-") as directory:
            journal = Path(directory) / "mock.jsonl"
            mock_rc = campaign.main(["--mock", "--log", str(journal)])
            rows = [json.loads(line) for line in journal.read_text(encoding="utf-8").splitlines()]
        results = [row for row in rows if row["event"] == "result"]
        result["offline_ok"] = offline_rc == 0 and offline["port_opened"] is False and len(offline["plan"]) == 12
        result["mock_ok"] = (mock_rc == 0 and rows[-1]["event"] == "complete" and
                             len(results) == 12 and all(row["simulated"] for row in rows))
        result["mock_nominal"] = sum(row["verdict"] == "exact_response" for row in results)
        result["mock_silences"] = sum(row["verdict"] == "expected_silence" for row in results)
    if not result["offline_ok"] or not result["mock_ok"]:
        result["errors"].append("offline/mock validation failed")
    result["files_sha256"] = {path.name: hashlib.sha256(path.read_bytes()).hexdigest()
                              for path in (Path(__file__), Path(campaign.__file__))}
    result["environment_ready"] = not result["errors"]
    result["remaining"] = "Confirm execution on the HP, exclusive master/unit1 topology, then independent ChatGPT GO review; do not transmit"
    return result


def main():
    result = inspect_environment()
    print(json.dumps(result, indent=2))
    return 0 if result["environment_ready"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
