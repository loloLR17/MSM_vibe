from pathlib import Path
import json,zlib,hashlib,re,shutil,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r5b2');r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B2-physical-resume');d=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-R5B2';sha=lambda b:hashlib.sha256(b).hexdigest()
a=json.loads((p/'image.json').read_text());(p/'image-prepared.json').write_text(json.dumps(a,indent=2)+'\n');a.update(flashed=True,readback_exact=True,firmware_source_sha='dc773733a98d78016e1d0f8a341b155e1851e2c7',controller_commit='7bf84b127ce3c473e340004b8d34a650594c45ed',original_build_reused=True)
for ext in ['bin','elf']:a[ext]['path']=str(p/'probe'/Path(a[ext]['path']).name)
a['final_physical_fram_sha256']=a.pop('predicted_fram_sha256_not_physical');(p/'image.json').write_text(json.dumps(a,indent=2)+'\n')
files=[x for x in p.iterdir() if x.is_file() and x.suffix!='.exe']+[x for x in (p/'hardware').iterdir() if x.is_file()]+list((p/'probe').iterdir())
bundle=[]
for f in sorted(files):
 if f.name.startswith('readback-'):continue
 rel=f.relative_to(p).as_posix();raw=f.read_bytes()
 for pattern in [rb'gh[pousr]_[a-zA-Z0-9]{30,}',rb'-----BEGIN (?:RSA |OPENSSH )?PRIVATE KEY-----',rb'github_pat_[A-Za-z0-9_]{50,}']:assert not re.search(pattern,raw),f
 # Preserve pre-flash checkpoint files byte-for-byte; all new Windows/large raw evidence compressed without normalization.
 old=d/rel
 if old.exists():assert old.read_bytes()==raw,rel;compress=False
 else:compress=len(raw)>18000 or b'\r\n' in raw or f.suffix in ['.bin','.elf','.db','.log'] or '.db-' in f.name
 dest=rel+('.zlib' if compress else '');b=zlib.compress(raw,9) if compress else raw;out=d/dest;out.parent.mkdir(parents=True,exist_ok=True)
 if out.exists():assert out.read_bytes()==b
 else:out.write_bytes(b)
 bundle.append(dict(source=rel,path=dest,encoding='zlib' if compress else 'raw',bytes=len(raw),sha256=sha(raw),stored_bytes=len(b),stored_sha256=sha(b)))
(d/'BUNDLE.json').write_text(json.dumps(dict(mission_id='TR2-20261018-R5B2-PHYSICAL-LIVE-RESUME',status='PHYSICAL_PASS_PUBLICATION_PENDING',artifacts=bundle),indent=2)+'\n')
(d/'README.md').write_text('''# R5-B2 — démonstration physique bornée, 18/10/2026UTC

Le service Windows local approuvé est identique aux372fichiers du buildR5B/dc773733, firmware inchangé également ; aucune reconstruction ni reprise des561tests. Contrôleur natif source7bf84b1, deux commandes nouvelles seulement5/12START/run11 et5/13STOP, maîtres sanschevauchement. Aucun contournement de politique Windows.

physical-summary.json/analyze.py :824ADU du vrai service/206B3,1033ADU maître explicite, tousCRC exacts ;234observationsCOM4SQLite dont88courantes/54HELD/28perteslogiques sanssnapshot/64initiales ;141lignesAPI corrélées ; DOM réelEdgeheadless en V/F. AucunFC10 du service. La base contient aussi294observationsCOM99 des deuxsmokes, explicitement distinctes etsansmesure. Les métadonnéesabsentes restentnull. Les548? chiffres ne sont pas extrapolés : utiliser les compteurs exacts du JSON et la base.

FlashM0162Mio restaurée exacte c5bc…f5b26, FRAMcomplète2b501a…7dd84 exacteaumodèle/relue aprèsrestauration sansrollback ; historique/tail inchangés, journal78/root91/epoch5/run11/config3gen3. Capteuroff/WHO7B, COMfermé/GDBdétaché. PasSD/HP/masserase/OB. Aucun produit permanentLIVE ni GO R6/industrie.

BUNDLE conserve les empreintes des originaux et des fichiers zlib. SHA256SUMS couvre tous les fichiers publiés. Les sources sous hardware et les logs sont les opérations effectivement exécutées cettefois ; source/copied image.json initiale distinguée dans image-prepared.json. Les captureswindows/HTTP brutes gardent leur encodage original ; browser-running-dom.html est duHTML renduUTF8, pas un screenshot ni observation humaine. Le premier helperHTTP échoué est préservé, deuxième collecte aprèsProxy=null bouclelocale réussie, code du service inchangé.
'''.replace(' Les548? chiffres ne sont pas extrapolés : utiliser les compteurs exacts du JSON et la base.',''))
(d/'SHA256SUMS').write_text(''.join(sha(x.read_bytes())+'  '+x.relative_to(d).as_posix()+'\n' for x in sorted(d.rglob('*')) if x.is_file() and x.name!='SHA256SUMS'))
print('BUNDLE',len(bundle),'files',len([x for x in d.rglob('*') if x.is_file()]))
