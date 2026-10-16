"""Expand the lossless, SHA-checked R4 STARTED archive; never access hardware."""
from pathlib import Path
import hashlib,subprocess,sys,tempfile,zlib
raw=zlib.decompress(Path(sys.argv[2]).read_bytes())
assert len(raw)==262144
assert hashlib.sha256(raw).hexdigest()=="c814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561"
with tempfile.TemporaryDirectory(prefix="tr2-r4m-") as directory:
 snapshot=Path(directory)/"fram.bin";snapshot.write_bytes(raw)
 subprocess.run([sys.argv[1],str(snapshot)],check=True)
