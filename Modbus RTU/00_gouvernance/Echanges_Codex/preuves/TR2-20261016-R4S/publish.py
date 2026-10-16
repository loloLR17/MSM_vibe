from pathlib import Path
import json,hashlib,zlib,shutil
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4S-publication');p=Path('/home/lolo/dev/msm/reports/TR2-r4s');proof=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4S';proof.mkdir(exist_ok=True);(proof/'hardware').mkdir(exist_ok=True)
for f in p.iterdir():
 if f.is_file() and f.suffix in {'.json','.log','.py','.c'}:
  if f.name=='initial-state.json':(proof/(f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
  else:shutil.copy2(f,proof/f.name)
for f in (p/'hardware').iterdir():
 if f.is_file() and f.suffix in {'.json','.log','.py','.gdb'}:shutil.copy2(f,proof/'hardware'/f.name)
 if f.is_file() and f.suffix=='.bin' and not f.name.startswith(('flash-','restore-')):
  (proof/'hardware'/(f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
for d in sorted(p.glob('probe-build-*')):
 if not d.is_dir() or d.name not in {'probe-build-01','probe-build-02','probe-build-03'}:continue
 for f in d.glob('*.bin'):(proof/(d.name+'-'+f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
 for f in d.glob('*.elf'):(proof/(d.name+'-'+f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
(proof/'README.md').write_text('''# R4-S — preuves par essai

Les phases contractuelles et tentatives sont distinguées dans le rapport.
Les images dédiées BIN et ELF sont conservées sans perte (.zlib : zlib.decompress).
Les sauvegardes Flash complètes restent hors Git sous reports/TR2-r4s/hardware.
Aucune valeur diagnostique à identité volatile ne vaut admission R2 : V/F retirés.
Scripts historiques : ne pas relancer sur un autre état, vérifier image/SHA/cible.
Les temps modèles hôte sont distincts des observations STM32 et des horloges SI.
Logs bruts Windows CRLF conservés ; diff whitespace vérifié sur code/docs.
Les logs maintenance/recovery/probe-build-04..06 et model-hashes relèvent d'une
étude hôte NON AUTORISÉE SUR CIBLE ; code local86d0f4b, image04 jamais flashée
ni exportée ici. Ils ne prouvent aucune écriture physique ni conformité phase2.
Le contre-exemple indépendant counterexample_model.c utilise main inchangé ;
son application générique sur RAM ne vaut pas autorisation de capture.
Restoration-summary.json décrit l'état final matériel, aucune phase2/3 physique.
SHA256SUMS couvre tous les fichiers sauf lui-même.
''')
files={str(f.relative_to(proof)):dict(bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in sorted(proof.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.json'}
(proof/'SHA256SUMS.json').write_text(json.dumps(dict(files=files),indent=2)+'\n');print('PROOFS',len(files))
