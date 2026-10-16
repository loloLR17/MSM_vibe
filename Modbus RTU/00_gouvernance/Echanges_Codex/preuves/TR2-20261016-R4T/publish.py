from pathlib import Path
import json,hashlib,zlib,shutil
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4T-monotonic-recovery');p=Path('/home/lolo/dev/msm/reports/TR2-r4t');q=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4T';q.mkdir(exist_ok=True);(q/'hardware').mkdir(exist_ok=True)
for f in p.iterdir():
 if f.is_file() and f.suffix in {'.json','.py','.log'}:
  if f.name=='initial-state.json':(q/(f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
  else:shutil.copy2(f,q/f.name)
 if f.name in {'model-admitted.bin','model-renew.bin'}:(q/(f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
for f in (p/'hardware').iterdir():
 if f.suffix in {'.json','.py','.gdb','.log'}:shutil.copy2(f,q/'hardware'/f.name)
 if f.suffix=='.bin' and not f.name.startswith(('flash-','restore-')):(q/'hardware'/(f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
for d in p.glob('probe-build-*'):
 if d.is_dir():
  for f in d.iterdir():
   if f.suffix in {'.elf','.bin'}:(q/(d.name+'-'+f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
(q/'README.md').write_text('''# R4-T — preuves

Les snapshots model-* sont des simulations sur les vrais stores, jamais écrits
sur cible. hardware/* décrit les observations physiques distinctes. Les images
BIN/ELF et snapshots .zlib se décompressent sans perte avec zlib.decompress.
Les sauvegardes Flash2Mio restent hors Git dans reports/TR2-r4t/hardware.
Ne pas relancer les scripts historiques sans préflight/autorisation/état exact.
Aucun loopback RAM ne vaut RS485 électrique. Publication != qualification indépendante.
Logs bruts Windows CRLF conservés. Manifeste SHA256 de tous les fichiers sauf lui-même.
''')
files={str(f.relative_to(q)):dict(bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in sorted(q.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.json'}
(q/'SHA256SUMS.json').write_text(json.dumps(dict(files=files),indent=2)+'\n');print('PROOFS',len(files))
