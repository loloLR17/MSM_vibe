"""Secondary schema2 only; old schema1 may be read separately, never migrated."""
import argparse,csv,hashlib,json,math,sqlite3
from datetime import datetime,timezone
from pathlib import Path
from core import canonical
APP=1380270914

def sha(p):
    with Path(p).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()

def counts(c):return {t:c.execute('SELECT count(*) FROM '+t).fetchone()[0] for t in ['campaigns','metric_definitions','analysis_runs','window_metrics','artifact_refs']}

def finite(v):
    f=float(v);return f if math.isfinite(f) else None

def import_results(directory,database):
    directory,database=Path(directory).resolve(),Path(database).resolve()
    m=json.loads((directory/'manifest.json').read_text());defs=json.loads((directory/'definitions.json').read_text())
    if m['normative_names_enabled'] or m['normative_thresholds_enabled']:raise ValueError('P0 blocked: normative import refused')
    if m['source']['status']!='COMPLETE' or sha(m['source']['path'])!=m['whole_file_sha256']:raise ValueError('source missing/corrupt/not COMPLETE')
    entries={e['name']:e for e in json.loads((directory/'artifacts.json').read_text())}
    artifacts=['metrics.csv','definitions.json','summary.json','clock_sensitivity.csv','fir_response.csv','real_crosschecks.json']
    for name in artifacts:
        if sha(directory/name)!=entries[name]['sha256']:raise ValueError('artifact changed: '+name)
    for mid,d in defs.items():
        if hashlib.sha256(canonical(d).encode()).hexdigest()!=mid:raise ValueError('metric definition identity mismatch')
        if d['edition']!='NONE_NON_NORMATIVE' or d['normative_equivalence_implemented']:raise ValueError('unverified normative definition refused')
    if database.exists():
        with sqlite3.connect(database.as_uri()+'?mode=ro',uri=True) as c:
            if (c.execute('pragma application_id').fetchone()[0],c.execute('pragma user_version').fetchone()[0])!=(APP,2):raise ValueError('foreign/schema1 database refused before writing')
    database.parent.mkdir(parents=True,exist_ok=True);c=sqlite3.connect(database)
    try:
        c.execute('PRAGMA foreign_keys=ON');c.execute('PRAGMA synchronous=FULL')
        if c.execute('PRAGMA user_version').fetchone()[0]==0:c.executescript('BEGIN IMMEDIATE;\n'+Path(__file__).with_name('schema_v2.sql').read_text()+'\nCOMMIT;')
        with (directory/'metrics.csv').open(newline='') as f:rows=list(csv.DictReader(f)) # aggregates only, ~2204 rows
        code=hashlib.sha256((m['algorithm_code_sha256']+m['core_code_sha256']).encode()).hexdigest();result=sha(directory/'metrics.csv');campaign=m['whole_file_sha256'];ids={};inserted=0
        with c:
            c.execute('BEGIN IMMEDIATE')
            c.execute('INSERT OR IGNORE INTO campaigns VALUES(?,?,?)',(campaign,m['source']['path'],canonical(m)))
            for mid,d in defs.items():
                aid=hashlib.sha256(canonical(dict(campaign=campaign,metric_definition_id=mid,code=code)).encode()).hexdigest();ids[mid]=aid
                old=c.execute('SELECT results_digest FROM analysis_runs WHERE analysis_id=?',(aid,)).fetchone()
                if old:
                    if old[0]!=result:raise ValueError('same method with conflicting aggregate results; refusing overwrite')
                    continue
                c.execute('INSERT OR IGNORE INTO metric_definitions VALUES(?,?,?,?,?,?,?,?)',(mid,d['version'],d['edition'],d['unit'],d['band'][0],d['band'][1],d['orientation']['description'],canonical(d)))
                c.execute('INSERT INTO analysis_runs VALUES(?,?,?,?,?,?)',(aid,campaign,mid,code,result,'PRELIMINARY_P0_BLOCKED'))
                for r in rows:
                    if r['metric_definition_id']!=mid:continue
                    if r['unit']!=d['unit']:raise ValueError('row/definition unit mismatch')
                    values=(aid,int(r['window_mcu_s']),float(r['start_mcu_s']),float(r['end_mcu_s']),int(r['observed_n']),int(r['valid_n']),float(r['coverage_observed']),r['quality'],*[finite(r[k]) for k in ['rms_x','rms_y','rms_z','direction_max','vector_technical','peak_vector']])
                    c.execute('INSERT INTO window_metrics VALUES('+','.join('?'*len(values))+')',values)
                for name in artifacts:c.execute('INSERT INTO artifact_refs VALUES(?,?,?,?)',(aid,name,str(directory/name),sha(directory/name)))
                inserted+=1
        if c.execute('pragma integrity_check').fetchone()[0]!='ok' or c.execute('pragma foreign_key_check').fetchall():raise ValueError('catalogue integrity failure')
        return dict(idempotent=inserted==0,analysis_ids=ids,counts=counts(c),integrity='ok',foreign_key_errors=[])
    finally:c.close()

def run_import(directory,database):
    try:return import_results(directory,database)
    except Exception as e:
        p=Path(str(database)+'.errors.jsonl');p.parent.mkdir(parents=True,exist_ok=True)
        with p.open('a') as f:f.write(canonical(dict(utc=datetime.now(timezone.utc).isoformat(),error_type=type(e).__name__,error=str(e)))+'\n')
        raise

QUERIES={
 'catalogue':'''SELECT d.unit,json_extract(d.definition_json,'$.clock.scenario') AS clock,w.window_mcu_s,count(*) AS windows FROM window_metrics w JOIN analysis_runs a USING(analysis_id) JOIN metric_definitions d USING(metric_definition_id) GROUP BY d.metric_definition_id,w.window_mcu_s ORDER BY d.unit,clock,w.window_mcu_s''',
 'worst_60s_velocity':'''SELECT start_mcu_s,direction_max,quality FROM window_metrics w JOIN analysis_runs a USING(analysis_id) JOIN metric_definitions d USING(metric_definition_id) WHERE d.unit='mm/s' AND json_extract(d.definition_json,'$.clock.scenario')='MCU' AND window_mcu_s=60 AND quality='FULL' ORDER BY direction_max DESC LIMIT 1''',
 'axes_120s':'''SELECT start_mcu_s,rms_x,rms_y,rms_z,quality FROM window_metrics w JOIN analysis_runs a USING(analysis_id) JOIN metric_definitions d USING(metric_definition_id) WHERE d.unit='mm/s' AND json_extract(d.definition_json,'$.clock.scenario')='MCU' AND window_mcu_s=120 ORDER BY start_mcu_s''',
 'method_status':'''SELECT normative_edition,axes_status,count(*) AS methods FROM metric_definitions GROUP BY normative_edition,axes_status'''
}

def queries(database):
    with sqlite3.connect(Path(database).resolve().as_uri()+'?mode=ro',uri=True) as c:
        c.row_factory=sqlite3.Row;out={}
        for name,sql in QUERIES.items():
            rows=[dict(r) for r in c.execute(sql)];out[name]=dict(count=len(rows),first_rows=rows[:3])
        return out

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('analysis',type=Path);p.add_argument('database',type=Path);a=p.parse_args();r=run_import(a.analysis,a.database);r['queries']=queries(a.database);print(json.dumps(r,indent=2))
