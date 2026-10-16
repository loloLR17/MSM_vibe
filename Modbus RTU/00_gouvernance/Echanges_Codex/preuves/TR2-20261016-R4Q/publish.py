from pathlib import Path
import json,hashlib,zlib,shutil,subprocess as s
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4Q-live-b3-rtu');local=Path('/home/lolo/dev/msm/reports/TR2-r4q');proof=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4Q';proof.mkdir(exist_ok=True);(proof/'hardware').mkdir(exist_ok=True)
code=s.check_output(['git','rev-parse','6e81350'],cwd=root,text=True).strip()
images={}
for label,bin in [('probe',local/'probe-build/tr2-r4q-readonly-rtu-NON-PRODUCTION.bin'),('canonical',root/'Modbus RTU/05_Firmware/build-stm32-cumulative/tr2-cumulative-m014-m015-spi4.bin')]:
 images[label]=dict(code_sha=code,bin_bytes=bin.stat().st_size,bin_sha256=hashlib.sha256(bin.read_bytes()).hexdigest(),elf_sha256=hashlib.sha256(bin.with_suffix('.elf').read_bytes()).hexdigest(),profile=(bin.parent/'build-profile.txt').read_text())
(proof/'probe.bin.zlib').write_bytes(zlib.compress((local/'probe-build/tr2-r4q-readonly-rtu-NON-PRODUCTION.bin').read_bytes(),9))
(local/'build-artifacts.json').write_text(json.dumps(images,indent=2)+'\n')
for f in local.iterdir():
 if f.is_file() and f.suffix in {'.json','.log','.py'}:shutil.copy2(f,proof/f.name)
for f in (local/'hardware').iterdir():
 if f.is_file() and f.suffix in {'.json','.log','.py','.gdb'}:shutil.copy2(f,proof/'hardware'/f.name)
 if f.is_file() and f.name.startswith('fram-') and f.suffix=='.bin':(proof/(f.name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
(proof/'README.md').write_text('''# R4-Q — preuves

Les horloges FIFO/modèle et allowances CPU des tests sont SIMULÉES. Les mesures
STM32 sont séparées dans hardware/*.log/json : RTU loopback RAM, jamais RS485
ni mesure vibratoire B3 physique. Le rapport précise les phases effectivement
exécutées. Les erreurs de construction des nouvelles fixtures restent conservées.
Snapshots .bin.zlib sans perte : zlib.decompress(),262144octets FRAM.
Backups Flash2Mio récupérables hors Git sous reports/TR2-r4q/hardware et anciens
backups R4P ; empreintes publiées. Probe NON-PRODUCTION, pas firmware opérationnel.
Logs Windows CRLF conservés intégraux. SHA256SUMS couvre tout sauf lui-même.
''')
files={str(f.relative_to(proof)):dict(bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in sorted(proof.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.json'}
(proof/'SHA256SUMS.json').write_text(json.dumps(dict(files=files),indent=2)+'\n');print('PROOFS',len(files))
