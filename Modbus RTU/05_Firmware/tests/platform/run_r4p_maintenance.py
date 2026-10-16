"""Expand the lossless, SHA-checked R4P STARTED2/run2 archive; never access hardware."""
from pathlib import Path
import hashlib,subprocess,sys,tempfile,zlib
raw=zlib.decompress(Path(sys.argv[2]).read_bytes())
assert len(raw)==262144
assert hashlib.sha256(raw).hexdigest()=="f2bd7a99d3d0e9f17292310f2cec51fe3a3f521ba3a4d2f07da5b6302258a0c1"
with tempfile.TemporaryDirectory(prefix="tr2-r4p-") as directory:
 snapshot=Path(directory)/"fram.bin";snapshot.write_bytes(raw)
 subprocess.run([sys.argv[1],str(snapshot)],check=True)
