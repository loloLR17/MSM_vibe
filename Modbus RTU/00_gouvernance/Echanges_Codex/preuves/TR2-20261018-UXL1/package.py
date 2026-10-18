from pathlib import Path
import hashlib,zlib,json,re
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-UXL1-live-readonly');p=Path('/home/lolo/dev/msm/reports/TR2-uxl1');d=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261018-UXL1';d.mkdir(parents=True,exist_ok=True);sha=lambda b:hashlib.sha256(b).hexdigest()
files=[f for f in p.rglob('*') if f.is_file() and f.suffix!='.exe'];bundle=[]
for f in sorted(files):
 raw=f.read_bytes();rel=f.relative_to(p).as_posix()
 for pat in [rb'gh[pousr]_[A-Za-z0-9]{30,}',rb'-----BEGIN (?:RSA |OPENSSH )?PRIVATE KEY-----']:assert not re.search(pat,raw),rel
 compress=len(raw)>20000 or f.suffix in ['.db','.trx','.log'];dest=rel+('.zlib' if compress else '');stored=zlib.compress(raw,9) if compress else raw;out=d/dest;out.parent.mkdir(parents=True,exist_ok=True);assert not out.exists();out.write_bytes(stored)
 bundle.append(dict(source=rel,path=dest,encoding='zlib' if compress else 'raw',bytes=len(raw),sha256=sha(raw),stored_bytes=len(stored),stored_sha256=sha(stored)))
(d/'BUNDLE.json').write_text(json.dumps(dict(mission_id='TR2-20261018-UXL1-LIVE-READONLY-INTEGRATION',physical_trial=False,artifacts=bundle),indent=2)+'\n')
(d/'README.md').write_text('''# UX-L1 — preuves logicielles et archives historiques

Aucune ouverture COM4, commande Modbus, Flash ou GDB. Copie de la vraie base R5-B2 (528 observations), utilisée uniquement comme HISTORIQUE : sourcebyte-identique6efaa…f0f5fd1, aucun WAL après consultation figée. Deux DOM Edge headless réels, parc et Vibrations, avec HELD, mg, sévérité0, provenance historique et NON-PRODUCTION. Aucun navigateur humain ou essai physique nouveau.

Logs et TRX conservent la première suite :350 hors Service PASS,214 Service PASS/1 échec nouveau sur WAL. Le mode figé adapté conserve l'assertion et refuse un WAL non vide ;215 Service PASS ensuite,217 après deux scénarios supplémentaires. Tests JS ciblés et smoke HTTP/API/POST404 réussis. BUNDLE contient les empreintes originales et stockées ; zlib sans réécriture des logs. Les scripts d'implémentation/smoke sont des preuves de travail, pas des commandes à relancer sur le matériel.
''')
(d/'SHA256SUMS').write_text(''.join(sha(f.read_bytes())+'  '+f.relative_to(d).as_posix()+'\n' for f in sorted(d.rglob('*')) if f.is_file() and f.name!='SHA256SUMS'))
print('proof artifacts',len(bundle))
