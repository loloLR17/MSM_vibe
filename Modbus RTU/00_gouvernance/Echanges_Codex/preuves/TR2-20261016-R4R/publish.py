from pathlib import Path
import json,hashlib,zlib,shutil
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4R-physical-time-b3');p=Path('/home/lolo/dev/msm/reports/TR2-r4r');proof=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4R';proof.mkdir(exist_ok=True);(proof/'hardware').mkdir(exist_ok=True)
for f in p.iterdir():
 if f.is_file() and f.suffix in {'.json','.log','.py','.c'}:
  if f.name=='initial-state.json':(proof/(f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
  else:shutil.copy2(f,proof/f.name)
for f in (p/'hardware').iterdir():
 if f.is_file() and f.suffix in {'.json','.log','.py','.gdb'}:shutil.copy2(f,proof/'hardware'/f.name)
 if f.is_file() and f.suffix=='.bin' and (f.name.startswith('fram-') or f.name in {'raw.bin','brackets.bin','raw02.bin','brackets02.bin','xyz.bin','valid-adu.bin','held-adu.bin'}):
  (proof/'hardware'/(f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
for b,label in [('probe-build','diagnostic-attempt01'),('probe-build-02','candidate-probe')]:
 f=p/b/'tr2-r4r-physical-time-NON-PRODUCTION.bin'
 if f.exists():(proof/(label+'.bin.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
(proof/'event-probe.bin.zlib').write_bytes(zlib.compress((p/'hardware/event-probe-readback.bin').read_bytes(),9))
if (p/'hardware/calibration/time-analysis.json').exists():shutil.copy2(p/'hardware/calibration/time-analysis.json',proof/'hardware/calibration-time-analysis.json')
(proof/'README.md').write_text('''# R4-R — preuves distinctes

Tentative01 interrompue par une relecture GDB prématurée : jamais PASS ; erreur
SPI et arrêt sûr conservés. Diagnostic complet deuxième image : arrêt sur point
checkpoint avant relecture, capteur arrêté. Les observations de temps sont des
comparaisons capteur/DWT, pas une référence UTC ni une calibration externe SI.
Les campagnes et restaurations effectivement réalisées sont précisées dans le
rapport. .bin.zlib : zlib.decompress(), données brutes intactes. Backups Flash2Mio
hors Git sous reports/TR2-r4r/hardware, origine M016 vérifiée ; jamais effacés.
Aucun essai UART/RS485/PC/SD ; images dédiées NON-PRODUCTION.
SHA256SUMS couvre tous les fichiers sauf lui-même.
''')
files={str(f.relative_to(proof)):dict(bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in sorted(proof.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.json'}
(proof/'SHA256SUMS.json').write_text(json.dumps(dict(files=files),indent=2)+'\n');print('PROOFS',len(files))
