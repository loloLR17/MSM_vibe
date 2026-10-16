from pathlib import Path
import shutil,json,hashlib,zlib
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4M-live-fram-reconciliation');local=Path('/home/lolo/dev/msm/reports/TR2-r4m');proof=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4M'
for f in local.iterdir():
 if f.is_file() and f.suffix in {'.json','.log','.py'} and f.stat().st_size<300000 and f.name not in {'checkpoint.py','prepare-resume.py','initial-state.json'}:shutil.copy2(f,proof/f.name)
for f in (local/'hardware').iterdir():
 if f.is_file() and f.suffix in {'.json','.gdb','.log','.py'}:shutil.copy2(f,proof/'hardware'/f.name)
for name in ['r4m-image-readback.bin','r4m-resume-image.bin','fram-after-resume.bin','fram-after-reboot.bin','fram-after-restoration.bin']:
 f=local/'hardware'/name
 if f.exists():
  data=f.read_bytes();(proof/(name+'.zlib')).write_bytes(zlib.compress(data,9))
(proof/'README.md').write_text('''# Preuves R4-M

Le rapport courant et LIVE_R4M distinguent les deux exécutions physiques :
trigger4 = isolation unique puis dépassement1075us avant RENEW ; trigger6 = reprise
explicite du pont vérifié, sans second reprovisionnement. Les sorties GDB brutes,
commandes, invocations, SHA et snapshots sans perte permettent le contrôle indépendant.
Les fichiers expected-final-MODEL-ONLY restent des oracles RAM, jamais des relevés.

Ancien STARTED complet : ../../../../05_Firmware/tests/fixtures/r4_started_snapshot.zlib
(262144octets décompressés, SHA c814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561).
Les autres .zlib se décompressent avec Python zlib.decompress ; aucune perte.
Les doubles sauvegardes Flash et la lecture finale2Mio sont conservées localement
sous reports/TR2-r4m/hardware ; leurs empreintes et contrôles sont publiés.
Les snapshots intermédiaires/finaux FRAM représentent chacun262144octets.
Les deux images de probes NON-PRODUCTION sont conservées sans perte ; leurs SHA
sont dans le rapport. Leur présence ne vaut pas autorisation de les relancer.
`measurements.json` dérive des logs GDB ; ces derniers priment en cas d'écart.
`SHA256SUMS.json` vérifie tous les fichiers complets sauf lui-même.
Les archives R4 restent intactes ; `latest-readback.json` consigne la publication
précédemment relue (le SHA du commit final est fourni dans la réponse Codex).
Aucun RTU électrique, SD, provider temps capteur calibré ni qualification R6.
''')
files={str(f.relative_to(proof)):dict(bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in sorted(proof.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.json'}
(proof/'SHA256SUMS.json').write_text(json.dumps(dict(files=files),indent=2)+'\n');print('PROOFS',len(files))
