from pathlib import Path
import json,hashlib,zlib,subprocess,sqlite3,xml.etree.ElementTree as E,datetime
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B-live-to-web');p=Path('/home/lolo/dev/msm/reports/TR2-r5b');d=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R5B';d.mkdir(parents=True,exist_ok=True)
sha=lambda b:hashlib.sha256(b).hexdigest();code=subprocess.check_output(['git','rev-parse','HEAD'],cwd=r,text=True).strip()
ns={'t':'http://microsoft.com/schemas/VisualStudio/TeamTest/2010'};tests=[dict(file=x.name,**E.parse(x).find('.//t:Counters',ns).attrib) for x in (p/'test-results').glob('*.trx')];assert sum(int(x['passed']) for x in tests)==561 and all(x['failed']=='0' and x['notExecuted']=='0' for x in tests)
with sqlite3.connect(p/'linux-smoke.db') as c:
 assert c.execute('pragma integrity_check').fetchone()[0]=='ok'
 rows=c.execute('select payload_json from live_observation').fetchall();obs=[json.loads(x[0]) for x in rows]
 assert obs and all(x['Snapshot'] is None and x['Continuity']==6 and x['RunId'] is None and x['Epoch'] is None for x in obs)
 audit=dict(rows=len(rows),all_communication_lost=True,all_snapshot_null=True,all_run_epoch_null=True,integrity='ok',real_serial_open=False,fixture='Linux intentionally nonexistent serial path; no physical measurement')
(p/'linux-smoke-sqlite-audit.json').write_text(json.dumps(audit,indent=2)+'\n')
(p/'test-summary.json').write_text(json.dumps(dict(total=561,failed=0,skipped=0,assemblies=tests,ui_scenarios=7,browser_graphical=False,firmware_146_prior_tests='reused unchanged core; not rerun',code_sha=code),indent=2)+'\n')
manifest=[]
for x in sorted((p/'windows').rglob('*')):
 if x.is_file():manifest.append(dict(path=x.relative_to(p/'windows').as_posix(),bytes=x.stat().st_size,sha256=sha(x.read_bytes())))
(p/'windows-build-manifest.json').write_text(json.dumps(dict(code_sha=code,rid='win-x64',self_contained=True,files=manifest,execution='BLOCKED Code Integrity 3033/3077; no security policy modified'),indent=2)+'\n')
files=[x for x in p.iterdir() if x.is_file() and x.suffix not in ['.exe'] and x.name not in ['model-final']]
files += [x for x in (p/'hardware').iterdir() if x.is_file()]
files += list((p/'test-results').glob('*.trx'))
files += [p/'probe'/('tr2-r5b-live-to-web-NON-PRODUCTION.'+ext) for ext in ['bin','elf']]+[p/'probe/build-profile.txt']
files += [r/'Modbus RTU/05_Firmware/build-stm32-cumulative'/n for n in ['artifact-sha256.txt','build-profile.txt']]
bundle=[]
for x in sorted(files):
 if x==Path(__file__): pass
 rel=x.relative_to(p).as_posix() if p in x.parents else 'canonical/'+x.name
 b=x.read_bytes();compress=len(b)>24000 or x.suffix in ['.bin','.elf','.db','.trx'];stored=zlib.compress(b,9) if compress else b;dest=rel+('.zlib' if compress else '');out=d/dest;out.parent.mkdir(parents=True,exist_ok=True)
 if out.exists():assert out.read_bytes()==stored,dest
 else:out.write_bytes(stored)
 bundle.append(dict(source=rel,path=dest,encoding='zlib' if compress else 'raw',bytes=len(b),sha256=sha(b),stored_bytes=len(stored),stored_sha256=sha(stored)))
(d/'BUNDLE.json').write_text(json.dumps(dict(mission_id='TR2-20261017-R5B-LIVE-PHYSICAL-TO-WEB-DEMO',code_sha=code,status='BLOCKED',physical_trial=False,artifacts=bundle),indent=2)+'\n')
(d/'README.md').write_text('''# R5-B — preuves du checkpoint logiciel et du blocage Windows

Code évalué `'''+code+'''`. Aucun essai physique LIVE R5-B, aucune ouverture COM4, aucun flash/reset/écriture FRAM. Préflight GDB lecture seule frais : Flash/FRAM complètes conformes R5AFIX et capteur arrêté. Les BIN/ELF R5B sont préparés, jamais flashés ; model-final.bin est un modèle hôte, jamais à restaurer sur cible.

561 tests .NET / sept scénarios JS simulés passent. Le service Linux avec chemin série volontairement inexistant sert HTTP200 et archive uniquement CommunicationLost/snapshot=null ; ce smoke n'est pas une acquisition. Le lancement Windows sur COM99 est refusé par Code Integrity avant la moindre ouverture série. Événements3033/3077 et erreur0x800711C7 conservés, aucune règle modifiée. Les images, sauvegardes, manifestes et sorties effectives sont contrôlables via BUNDLE.json et SHA256SUMS.

Les sources sous hardware contiennent des préparatifs hérités de restauration, NON exécutés sauf preflight.py/hardware.py ; elles ne constituent pas un contrôleur R5B qualifié. La reprise doit reprendre le plan LIVE_R5B.md après résolution autorisée du déploiement Windows. Ne jamais lancer d'ancienne recette R5AFIX avec les identités consommées5/10–5/11 ni charger le modèle FRAM.
''')
(d/'SHA256SUMS').write_text(''.join(sha(x.read_bytes())+'  '+x.relative_to(d).as_posix()+'\n' for x in sorted(d.rglob('*')) if x.is_file() and x.name!='SHA256SUMS'))
print(dict(proof_files=len(list(d.rglob('*'))),artifacts=len(bundle),sqlite=audit,code=code))
