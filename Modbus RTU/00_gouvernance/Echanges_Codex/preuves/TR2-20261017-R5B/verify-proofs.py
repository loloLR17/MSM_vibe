from pathlib import Path
import json,zlib,hashlib,sqlite3,re
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R5B-live-to-web');p=Path('/home/lolo/dev/msm/reports/TR2-r5b');d=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261017-R5B'
sha=lambda x:hashlib.sha256(x).hexdigest()
meta=json.loads((d/'BUNDLE.json').read_text());assert len(meta['artifacts'])==76
for x in meta['artifacts']:
 b=(d/x['path']).read_bytes();assert len(b)==x['stored_bytes'] and sha(b)==x['stored_sha256'];raw=zlib.decompress(b) if x['encoding']=='zlib' else b;assert len(raw)==x['bytes'] and sha(raw)==x['sha256']
 for pat in [rb'gh[pousr]_[a-zA-Z0-9]{30,}',rb'-----BEGIN (?:RSA |OPENSSH )?PRIVATE KEY-----',rb'github_pat_[A-Za-z0-9_]{50,}']:
  assert not re.search(pat,raw),x['path']
for line in (d/'SHA256SUMS').read_text().splitlines():
 h,f=line.split('  ',1);assert sha((d/f).read_bytes())==h,f
print('PASS bundle76 artifacts, full hashes and secret-pattern scan; manifest exact')
