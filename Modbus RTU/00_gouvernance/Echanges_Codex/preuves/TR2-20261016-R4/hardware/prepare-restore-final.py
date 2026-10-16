from hardware import *
backup=(P/'flash-before.bin').read_bytes();assert len(backup)==2097152
assert hashlib.sha256(backup).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
# U575 regular 8192-byte pages: only the page span changed by these probes.
length=(max((P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.bin').stat().st_size,62312)+8191)//8192*8192
image=P/'restore-inherited-pages-final.bin';assert not image.exists();image.write_bytes(backup[:length])
record=dict(source='flash-before.bin',source_sha256=hashlib.sha256(backup).hexdigest(),address='0x08000000',page_bytes=8192,bytes=length,sha256=hashlib.sha256(image.read_bytes()).hexdigest(),normal_programming_only=True,mass_erase=False,justification='Restore exactly the page span changed by R4 programming, including original FF padding; preserve every untouched page. Full 2MiB readback follows.')
(P/'restore-final-plan.json').write_text(json.dumps(record,indent=2)+'\n');print(record)
