"""Derived schema5 copy only. Cumulative v1/v2/v3/v4 and LIVE never written."""
import sqlite3,csv,json,hashlib
from contextlib import closing
from wm import *
from prequal_catalog import readonly,fingerprints

def all_old(c):return fingerprints(c,exclude_prefix='wm_')

def create_copy(source,dest):
 source,dest=Path(source).resolve(),Path(dest).resolve()
 if source==dest or dest.exists():raise ValueError('distinct NEW copy required')
 before=sha(source)
 with closing(readonly(source)) as c:
  if (c.execute('PRAGMA application_id').fetchone()[0],c.execute('PRAGMA user_version').fetchone()[0])!=(1380270915,4):raise ValueError('expected cumulativev4')
  old=all_old(c)
  with dest.open('xb'):pass
  target=sqlite3.connect(dest)
  try:
   c.backup(target);target.executescript('''BEGIN IMMEDIATE;
CREATE TABLE wm_runs(analysis_id TEXT PRIMARY KEY,campaign_sha TEXT NOT NULL REFERENCES campaigns(campaign_id),code_sha TEXT NOT NULL,result_sha TEXT NOT NULL,manifest_json TEXT NOT NULL);
CREATE TABLE wm_methods(method_id TEXT PRIMARY KEY,definition_json TEXT NOT NULL);
CREATE TABLE wm_metrics(analysis_id TEXT NOT NULL REFERENCES wm_runs,method_id TEXT NOT NULL REFERENCES wm_methods,width_s INTEGER NOT NULL,start_mcu_s INTEGER NOT NULL,result_json TEXT NOT NULL,PRIMARY KEY(analysis_id,method_id,width_s,start_mcu_s));
CREATE TABLE wm_audit(kind TEXT PRIMARY KEY,definition_json TEXT NOT NULL);
PRAGMA user_version=5;
COMMIT;''')
   if all_old(target)!=old:raise ValueError('legacy copy mismatch')
   target.execute('INSERT INTO wm_audit VALUES(?,?)',('source_v4',canonical(dict(path=str(source),sha256=before,legacy_fingerprints=old))));target.commit()
  finally:target.close()
 if sha(source)!=before:raise ValueError('original changed')
 return dict(source_sha256=before,legacy_fingerprints=old)

def import_results(directory,database):
 directory,database=Path(directory),Path(database);m=json.loads((directory/'MANIFEST.json').read_text());defs=json.loads((directory/'definitions.json').read_text())
 for mid,d in defs.items():
  if hashlib.sha256(canonical(d).encode()).hexdigest()!=mid or d['normative_compliance'] or d['metric'] not in ['TR2_WM_A_MAX','TR2_WM_DIAGNOSTIC_LOW08','TR2_WM_DIAGNOSTIC_LOW12']:raise ValueError('unknown Wm method')
 code=hashlib.sha256((m['core_sha256']+m['runner_sha256']).encode()).hexdigest();digest=sha(directory/'metrics.csv');aid=hashlib.sha256(canonical(dict(campaign=m['source_whole_sha256'],code=code,methods=sorted(defs))).encode()).hexdigest()
 with closing(readonly(database)) as c:
  if (c.execute('PRAGMA application_id').fetchone()[0],c.execute('PRAGMA user_version').fetchone()[0])!=(1380270915,5):raise ValueError('expected derivedv5')
  preserved=all_old(c)
 c=sqlite3.connect(database)
 try:
  c.execute('PRAGMA foreign_keys=ON')
  with c:
   previous=c.execute('SELECT result_sha FROM wm_runs WHERE analysis_id=?',(aid,)).fetchone()
   if previous and previous[0]!=digest:raise ValueError('refuse conflicting results')
   if not previous:
    c.execute('INSERT INTO wm_runs VALUES(?,?,?,?,?)',(aid,m['source_whole_sha256'],code,digest,canonical(m)))
    for mid,d in defs.items():
     existing=c.execute('SELECT definition_json FROM wm_methods WHERE method_id=?',(mid,)).fetchone()
     if existing and existing[0]!=canonical(d):raise ValueError('method collision')
     c.execute('INSERT OR IGNORE INTO wm_methods VALUES(?,?)',(mid,canonical(d)))
    with (directory/'metrics.csv').open() as stream:
     for r in csv.DictReader(stream):
      if r['method_id'] not in defs or r['unit']!='mm/s2':raise ValueError('row identity/units')
      nums={k:float(v) if v else None for k,v in r.items() if k in ['rms_x','rms_y','rms_z','direction_max','TR2_WM_A_MAX','equivalent_velocity_mm_s']}
      c.execute('INSERT INTO wm_metrics VALUES(?,?,?,?,?)',(aid,r['method_id'],int(r['width_s']),int(r['start_mcu_s']),canonical({**r,**nums})))
  if all_old(c)!=preserved:raise ValueError('historical cohort changed')
  integrity=c.execute('PRAGMA integrity_check').fetchone()[0];fk=c.execute('PRAGMA foreign_key_check').fetchall()
  if integrity!='ok' or fk:raise ValueError('database integrity')
  return dict(analysis_id=aid,idempotent=bool(previous),rows=c.execute('SELECT count(*) FROM wm_metrics').fetchone()[0],methods=c.execute('SELECT count(*) FROM wm_methods').fetchone()[0],integrity=integrity,FK_errors=fk,all_historical_tables_preserved=True)
 finally:c.close()

def register(database,aid,paths):
 with closing(readonly(database)) as c:
  if c.execute('PRAGMA user_version').fetchone()[0]!=5:raise ValueError('expected derivedv5')
 c=sqlite3.connect(database)
 try:
  with c:
   if not c.execute('SELECT 1 FROM wm_runs WHERE analysis_id=?',(aid,)).fetchone():raise ValueError('unknown analysis')
   for p in map(Path,paths):
    key=aid+':'+p.name;value=canonical(dict(path=str(p.resolve()),sha256=sha(p),bytes=p.stat().st_size))
    previous=c.execute('SELECT definition_json FROM wm_audit WHERE kind=?',(key,)).fetchone()
    if previous and previous[0]!=value:raise ValueError('refuse artifact overwrite')
    c.execute('INSERT OR IGNORE INTO wm_audit VALUES(?,?)',(key,value))
  return dict(audits=c.execute('SELECT count(*) FROM wm_audit').fetchone()[0],integrity=c.execute('PRAGMA integrity_check').fetchone()[0])
 finally:c.close()
