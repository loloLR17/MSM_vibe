"""Single derived v4 copy; originals v1/v2/v3 are strictly read-only."""
import sqlite3,csv
from contextlib import closing
from prequal import *

def readonly(path):return sqlite3.connect(Path(path).resolve().as_uri()+'?mode=ro',uri=True)

def fingerprints(c,exclude_prefix='prequal_'):
 result={}
 for name,sql in c.execute("SELECT name,sql FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name").fetchall():
  if name.startswith(exclude_prefix):continue
  quote='"'+name.replace('"','""')+'"';cols=c.execute('PRAGMA table_info('+quote+')').fetchall();order=','.join('"'+r[1].replace('"','""')+'"' for r in cols)
  h=hashlib.sha256();n=0
  for row in c.execute('SELECT * FROM '+quote+' ORDER BY '+order):h.update(repr(tuple(row)).encode()+b'\n');n+=1
  result[name]=dict(rows=n,sha256=h.hexdigest(),schema_sql=sql)
 return result

def create_copy(source,dest):
 source,dest=Path(source).resolve(),Path(dest).resolve()
 if source==dest or dest.exists():raise ValueError('new distinct derived database required')
 before=sha(source)
 with closing(readonly(source)) as c:
  if (c.execute('PRAGMA application_id').fetchone()[0],c.execute('PRAGMA user_version').fetchone()[0])!=(1380270915,3):raise ValueError('expected published schema3')
  old=fingerprints(c)
  with dest.open('xb'):pass
  target=sqlite3.connect(dest)
  try:
   c.backup(target)
   target.executescript('''BEGIN IMMEDIATE;
CREATE TABLE prequal_runs(analysis_id TEXT PRIMARY KEY,campaign_sha TEXT NOT NULL REFERENCES campaigns(campaign_id),code_sha TEXT NOT NULL,result_sha TEXT NOT NULL,manifest_json TEXT NOT NULL);
CREATE TABLE prequal_methods(method_id TEXT PRIMARY KEY,definition_json TEXT NOT NULL);
CREATE TABLE prequal_metrics(analysis_id TEXT NOT NULL REFERENCES prequal_runs,method_id TEXT NOT NULL REFERENCES prequal_methods,width_s INTEGER NOT NULL,start_mcu_s INTEGER NOT NULL,result_json TEXT NOT NULL,PRIMARY KEY(analysis_id,method_id,width_s,start_mcu_s));
CREATE TABLE prequal_audit(kind TEXT PRIMARY KEY,definition_json TEXT NOT NULL);
PRAGMA user_version=4;
COMMIT;''')
   if fingerprints(target)!=old:raise ValueError('copy changed legacy data')
   target.execute('INSERT INTO prequal_audit VALUES(?,?)',('source_v3',canonical(dict(path=str(source),sha256=before,legacy_fingerprints=old))));target.commit()
  finally:target.close()
 assert sha(source)==before
 return dict(source_sha256=before,legacy_tables=len(old),legacy_fingerprints=old)

def import_results(directory,database):
 directory,database=Path(directory),Path(database)
 m=json.loads((directory/'MANIFEST.json').read_text());defs=json.loads((directory/'definitions.json').read_text())
 for mid,d in defs.items():
  assert hashlib.sha256(canonical(d).encode()).hexdigest()==mid and not d['normative']
 code=hashlib.sha256((m['core_sha256']+m['runner_sha256']).encode()).hexdigest();result=sha(directory/'metrics.csv');aid=hashlib.sha256(canonical(dict(campaign=m['source_whole_sha256'],code=code)).encode()).hexdigest()
 with closing(readonly(database)) as c:
  if (c.execute('PRAGMA application_id').fetchone()[0],c.execute('PRAGMA user_version').fetchone()[0])!=(1380270915,4):raise ValueError('expected derived v4')
  preserved=fingerprints(c)
 c=sqlite3.connect(database)
 try:
  c.execute('PRAGMA foreign_keys=ON');c.execute('PRAGMA synchronous=FULL')
  with c:
   old=c.execute('SELECT result_sha FROM prequal_runs WHERE analysis_id=?',(aid,)).fetchone()
   if old and old[0]!=result:raise ValueError('conflicting results for same identity')
   if not old:
    c.execute('INSERT INTO prequal_runs VALUES(?,?,?,?,?)',(aid,m['source_whole_sha256'],code,result,canonical(m)))
    for mid,d in defs.items():
     previous=c.execute('SELECT definition_json FROM prequal_methods WHERE method_id=?',(mid,)).fetchone()
     if previous and previous[0]!=canonical(d):raise ValueError('method collision')
     c.execute('INSERT OR IGNORE INTO prequal_methods VALUES(?,?)',(mid,canonical(d)))
    with (directory/'metrics.csv').open() as f:
     for r in csv.DictReader(f):
      if r['method_id'] not in defs:raise ValueError('unknown method')
      numeric={k:float(v) for k,v in r.items() if k in ['rms_x','rms_y','rms_z','direction_max','vector_technical']}
      c.execute('INSERT INTO prequal_metrics VALUES(?,?,?,?,?)',(aid,r['method_id'],int(r['width_s']),int(r['start_mcu_s']),canonical({**r,**numeric})))
  if fingerprints(c)!=preserved:raise ValueError('historical cohort altered')
  integrity=c.execute('PRAGMA integrity_check').fetchone()[0];fk=c.execute('PRAGMA foreign_key_check').fetchall();assert integrity=='ok' and not fk
  return dict(analysis_id=aid,idempotent=bool(old),rows=c.execute('SELECT count(*) FROM prequal_metrics').fetchone()[0],methods=c.execute('SELECT count(*) FROM prequal_methods').fetchone()[0],integrity=integrity,FK_errors=fk,historical_tables_preserved=True)
 finally:c.close()

def register_artifacts(database,analysis_id,paths):
 """Append compact provenance without replacing an earlier artifact identity."""
 with closing(readonly(database)) as check:
  if check.execute('PRAGMA user_version').fetchone()[0]!=4:raise ValueError('expected derived v4')
 c=sqlite3.connect(database)
 try:
  with c:
   if not c.execute('SELECT 1 FROM prequal_runs WHERE analysis_id=?',(analysis_id,)).fetchone():raise ValueError('unknown analysis')
   for p in map(Path,paths):
    kind='artifact:'+analysis_id+':'+p.name;value=canonical(dict(path=str(p.resolve()),bytes=p.stat().st_size,sha256=sha(p)))
    previous=c.execute('SELECT definition_json FROM prequal_audit WHERE kind=?',(kind,)).fetchone()
    if previous and previous[0]!=value:raise ValueError('refuse replacing artifact audit')
    c.execute('INSERT OR IGNORE INTO prequal_audit VALUES(?,?)',(kind,value))
  return dict(artifact_audits=c.execute("SELECT count(*) FROM prequal_audit WHERE kind LIKE 'artifact:%'").fetchone()[0],integrity=c.execute('PRAGMA integrity_check').fetchone()[0])
 finally:c.close()
