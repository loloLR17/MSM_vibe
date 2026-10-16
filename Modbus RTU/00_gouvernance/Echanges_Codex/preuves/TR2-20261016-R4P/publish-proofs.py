from pathlib import Path
import shutil,json,hashlib,zlib,subprocess as s
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4P-physical-coexistence');local=Path('/home/lolo/dev/msm/reports/TR2-r4p');proof=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4P'
artifacts={}
for label,directory,ref in [('initial','bench-build','369887b'),('corrected','resume-build','9f429e5')]:
 image=local/(directory+'/tr2-r4p-coexistence-NON-PRODUCTION.bin');elf=image.with_suffix('.elf')
 artifacts[label]=dict(code_sha=s.check_output(['git','rev-parse',ref],cwd=root,text=True).strip(),bin_bytes=image.stat().st_size,bin_sha256=hashlib.sha256(image.read_bytes()).hexdigest(),elf_sha256=hashlib.sha256(elf.read_bytes()).hexdigest(),profile=(image.parent/'build-profile.txt').read_text())
artifacts['canonical_sha256']=hashlib.sha256((root/'Modbus RTU/05_Firmware/build-stm32-cumulative/tr2-cumulative-m014-m015-spi4.bin').read_bytes()).hexdigest()
assert artifacts['canonical_sha256']=='6a2150af8939ead0b81219a96cfd51dc2cbef03a3851d46775fa71ddbfeb15a9'
r=artifacts
(local/'build-artifacts.json').write_text(json.dumps(r,indent=2)+'\n')
for f in local.iterdir():
 if f.is_file() and f.suffix in {'.json','.log','.py'} and f.stat().st_size<300000:shutil.copy2(f,proof/f.name)
for f in (local/'hardware').iterdir():
 if f.is_file() and f.suffix in {'.json','.gdb','.log','.py'}:shutil.copy2(f,proof/'hardware'/f.name)
for name in ['fram-after-physical.bin','fram-after-readonly.bin','fram-after-resume.bin','fram-after-reboot.bin','fram-after-restoration.bin']:
 f=local/'hardware'/name
 if f.exists():(proof/(name+'.zlib')).write_bytes(zlib.compress(f.read_bytes(),9))
(proof/'corrected-probe.bin.zlib').write_bytes(zlib.compress((local/'resume-build/tr2-r4p-coexistence-NON-PRODUCTION.bin').read_bytes(),9))
(proof/'README.md').write_text('''# R4-P — preuves

Le rapport courant distingue les étapes acquises, les erreurs et les opérations
non exécutées. Logs GDB bruts, commandes/invocations, source/build/empreintes,
snapshots FRAM sans perte et comparaisons complètes servent au contrôle indépendant.
Les identifiants r4m_* des variables réutilisées ne désignent pas un ancien essai :
les scripts r4p-* et le code/build-artifacts.json identifient cette campagne R4-P.
expected-final-MODEL-ONLY est un oracle RAM, jamais un relevé STM32.
La trace initiale authentique STARTED2/run2 reste intégralement archivée dans
../../../../05_Firmware/tests/fixtures/r4p_started_snapshot.zlib (SHA f2bd7a99…8a0c1).
Les .zlib sont sans perte (zlib.decompress),262144octets pour chaque snapshot FRAM.
Les doubles backups Flash2Mio et FRAM et la lecture Flash restaurée sont récupérables
sous /home/lolo/dev/msm/reports/TR2-r4p/hardware/ ; hashes/contrôles publiés.
L'image du probe NON-PRODUCTION est fournie pour vérification, pas pour exploitation.
SHA256SUMS.json vérifie tous les fichiers complets sauf lui-même. CRLF Windows bruts
conservés. Publication/relecture ne valent pas qualification indépendante R4/R6.
''')
files={str(f.relative_to(proof)):dict(bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in sorted(proof.rglob('*')) if f.is_file() and f.name!='SHA256SUMS.json'};(proof/'SHA256SUMS.json').write_text(json.dumps(dict(files=files),indent=2)+'\n');print('PROOFS',len(files))
