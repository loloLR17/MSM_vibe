#!/usr/bin/env python3
"""Offline only: decode saved A04-02 record using repository C implementation."""
from pathlib import Path
import hashlib, json, subprocess, tempfile
proof = Path(__file__).resolve().parent
root = next(parent for parent in proof.parents if (parent / "AGENTS.md").is_file())
fw = root / "Modbus RTU/05_Firmware"
sources = [proof / "authority_probe.c", fw / "src/persistence/configuration_record.c", fw / "src/domain/configuration/configuration_validator.c"]
record = bytes.fromhex((proof / "configuration-slot1.hex").read_text())
assert len(record) == 140
with tempfile.TemporaryDirectory(prefix="m016-authority-") as directory:
    executable = Path(directory) / "probe"
    command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-I", str(fw / "include"), *map(str, sources), "-o", str(executable)]
    compiled = subprocess.run(command, capture_output=True, text=True)
    assert compiled.returncode == 0, compiled.stderr
    tested = subprocess.run([str(executable), str(proof / "configuration-slot1.hex")], capture_output=True, text=True)
    assert tested.returncode == 0, tested.stdout + tested.stderr
    print(tested.stdout, end="")
    metadata = {"kind": "OFFLINE_SAVED_RECORD_C_PROBE", "compiler": subprocess.check_output(["cc", "--version"], text=True).splitlines()[0], "compile_returncode": compiled.returncode, "probe_returncode": tested.returncode, "record_bytes": len(record), "record_sha256": hashlib.sha256(record).hexdigest(), "record_origin": "A04-02 saved boot RAM candidate, slot1 offset140; not a new FRAM observation", "source_sha256": {str(source.relative_to(root)): hashlib.sha256(source.read_bytes()).hexdigest() for source in sources}, "stdout": tested.stdout, "stderr": tested.stderr, "hardware_operations": 0}
    (proof / "authority-probe-result.json").write_text(json.dumps(metadata, indent=2) + "\n")
