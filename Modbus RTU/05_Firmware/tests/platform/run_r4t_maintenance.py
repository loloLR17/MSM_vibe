"""Expand the lossless, SHA-checked R4 STARTED archive; never access hardware."""
from pathlib import Path
import hashlib,subprocess,sys,tempfile,zlib
raw=zlib.decompress(Path(sys.argv[2]).read_bytes())
assert len(raw)==262144
assert hashlib.sha256(raw).hexdigest()=="14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9"
with tempfile.TemporaryDirectory(prefix="tr2-r4t-") as directory:
 snapshot=Path(directory)/"fram.bin";snapshot.write_bytes(raw)
 subprocess.run([sys.argv[1],str(snapshot)],check=True)
