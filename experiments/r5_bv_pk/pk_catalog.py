"""Version3 unified COPY of schema2, originals opened read-only and left unchanged."""
import argparse,csv,gzip,hashlib,json,sqlite3
from pathlib import Path
from core import BANDS,THRESHOLDS
APP2=1380270914;APP3=1380270915

def canonical(x):return json.dumps(x,sort_keys=True,separators=(',',':'),ensure_ascii=True,allow_nan=False)

def sha(p):
    with Path(p).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()

def signature(x):return hashlib.sha256(canonical(x).encode()).hexdigest()

def ro(path):return sqlite3.connect(Path(path).resolve().as_uri()+'?mode=ro',uri=True)

def legacy_snapshot(c):
    result={}
    # Logical fingerprint of ALL rows in the four existing scientific tables.
    for table in ['campaigns','metric_definitions','analysis_runs','window_metrics','artifact_refs']:
        rows=c.execute('SELECT * FROM '+table+' ORDER BY rowid').fetchall()
        result[table]=dict(rows=len(rows),ordered_content_sha256=signature(rows))
    return result

def create_migrated_copy(source,destination):
    source,destination=Path(source).resolve(),Path(destination).resolve()
    if source==destination:raise ValueError('refuse migrating original in place')
    before=sha(source)
    with ro(source) as old:
        if (old.execute('PRAGMA application_id').fetchone()[0],old.execute('PRAGMA user_version').fetchone()[0])!=(APP2,2):raise ValueError('expected isolated schema2 only')
        snap=legacy_snapshot(old)
        if old.execute('PRAGMA integrity_check').fetchone()[0]!='ok':raise ValueError('old catalogue integrity')
        destination.parent.mkdir(parents=True,exist_ok=True)
        # Exclusive reservation: no existing file can be overwritten by backup().
        with destination.open('xb'):pass
        c=sqlite3.connect(destination)
        try:
            old.backup(c);c.execute('PRAGMA foreign_keys=ON');c.execute('PRAGMA synchronous=FULL')
            c.executescript('BEGIN IMMEDIATE;\n'+Path(__file__).with_name('schema_v3.sql').read_text()+'\nCOMMIT;')
            with c:c.execute('INSERT INTO pk_migration_audit VALUES(1,?,?,?)',(str(source),before,canonical(snap)))
            if legacy_snapshot(c)!=snap:raise ValueError('legacy data changed during copy/migration')
        finally:c.close()
    if sha(source)!=before:raise ValueError('source schema2 changed')
    return dict(source=str(source),source_sha256=before,legacy_snapshot=snap)


def typed_row(r):
    out={}
    strings={'method_id','clock','band','unit','axis','frame_status','status','comparison_zone'}
    integers={'sample_n','valid_input_n','window_mcu_s'}
    for k,v in r.items():
        if k=='comparison_eligible':out[k]=v=='True'
        elif v=='':out[k]=None
        elif k in strings:out[k]=v
        elif k in integers:out[k]=int(v)
        else:out[k]=float(v)
    return out


def import_results(directory,database):
    directory,database=Path(directory).resolve(),Path(database).resolve()
    m=json.loads((directory/'manifest.json').read_text());methods=json.loads((directory/'methods.json').read_text())
    if m['source']['status']!='COMPLETE' or sha(m['source']['path'])!=m['whole_file_sha256']:raise ValueError('source changed/not COMPLETE')
    listed={v['name']:v for v in json.loads((directory/'artifacts.json').read_text())}
    artifacts=['components.csv.gz','methods.json','summary.json','independent_real.json','fir_response.json']
    for name in artifacts:
        if sha(directory/name)!=listed[name]['sha256']:raise ValueError('PK artifact changed: '+name)
    with gzip.open(directory/'components.csv.gz','rt',newline='') as f:rows=[typed_row(r) for r in csv.DictReader(f)]
    with ro(database) as check:
        if (check.execute('PRAGMA application_id').fetchone()[0],check.execute('PRAGMA user_version').fetchone()[0])!=(APP3,3):raise ValueError('not a migrated COPY schema3')
    reference=dict(edition='BV_NR467_F_2026_07_PK',document_sha256='5ae75d30f6b38a1331431e16bcc3dce11bc42c90b393f344be89b42cb050ceef',document_url='https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf',clauses='Ch6Sec1 2.1.3(b), Sec3 Tables5/6',CF=1,values=THRESHOLDS,local_correspondence_confirmed=False,formal_grade_awarded=False)
    reference_id=signature(reference);ids={};inserted=0
    c=sqlite3.connect(database)
    try:
        c.execute('PRAGMA foreign_keys=ON');c.execute('PRAGMA synchronous=FULL')
        with c:
            c.execute('BEGIN IMMEDIATE')
            if not c.execute('SELECT 1 FROM campaigns WHERE campaign_id=?',(m['whole_file_sha256'],)).fetchone():raise ValueError('PK campaign not identical to migrated R5 campaign')
            c.execute('INSERT OR IGNORE INTO pk_reference VALUES(?,?)',(reference_id,canonical(reference)))
            for mid,d in methods.items():
                if signature(d)!=mid or d['edition']!='BV_NR467_F_2026_07_PK' or d['CF']!=1:raise ValueError('method/reference identity')
                selected=[r for r in rows if r['method_id']==mid]
                digest=signature(selected);aid=signature(dict(campaign=m['whole_file_sha256'],method=mid,runner=m['runner_sha256']));ids[mid]=aid
                old=c.execute('SELECT results_sha256 FROM pk_analysis_runs WHERE analysis_id=?',(aid,)).fetchone()
                if old:
                    if old[0]!=digest:raise ValueError('same PK method conflicting results')
                    continue
                c.execute('INSERT OR IGNORE INTO pk_methods VALUES(?,?,?,?,?,?,?,?)',(mid,d['band'],d['unit'],d['clock'],d['edition'],d['derived_method_version'],reference_id,canonical(d)))
                c.execute('INSERT INTO pk_analysis_runs VALUES(?,?,?,?,?,?)',(aid,m['whole_file_sha256'],mid,m['runner_sha256'],digest,canonical(m)))
                for r in selected:
                    if (r['band'],r['unit'],r['clock'])!=(d['band'],d['unit'],d['clock']):raise ValueError('row/method semantics differ')
                    vals=(aid,r['start_mcu_s'],r['end_mcu_s'],r['window_mcu_s'],r['axis'],r['sample_n'],r['valid_input_n'],r['frame_status'],r['status'],r['reported_frequency_Hz'] or r['frequency_Hz'],r['component_pk'],r['component_rms'],r['reported_pk'],r['contrast_dB'],r['shape_error'],int(r['comparison_eligible']),r['comparison_zone'],canonical(r))
                    c.execute('INSERT INTO pk_components VALUES('+','.join('?'*len(vals))+')',vals)
                for name in artifacts:c.execute('INSERT INTO pk_artifacts VALUES(?,?,?,?)',(aid,name,str(directory/name),sha(directory/name)))
                inserted+=1
            for r in json.loads((directory/'independent_real.json').read_text()):c.execute('INSERT OR IGNORE INTO pk_controls VALUES(?,?)',(signature(r),canonical(r)))
        if c.execute('PRAGMA integrity_check').fetchone()[0]!='ok' or c.execute('PRAGMA foreign_key_check').fetchall():raise ValueError('v3 integrity')
        audit=json.loads(c.execute('SELECT source_snapshot_json FROM pk_migration_audit').fetchone()[0])
        if legacy_snapshot(c)!=audit:raise ValueError('v2 data not preserved')
        counts={t:c.execute('SELECT count(*) FROM '+t).fetchone()[0] for t in ['campaigns','window_metrics','pk_reference','pk_methods','pk_analysis_runs','pk_components','pk_controls']}
        return dict(idempotent=inserted==0,analysis_ids=ids,counts=counts,integrity='ok',foreign_key_errors=[],legacy_logically_preserved=True)
    finally:c.close()

QUERIES={
 'pk_inventory':'''SELECT a.analysis_id,m.band,m.clock,p.window_mcu_s,count(*) AS components FROM pk_components p JOIN pk_analysis_runs a USING(analysis_id) JOIN pk_methods m USING(method_id) GROUP BY a.analysis_id,m.band,m.clock,p.window_mcu_s ORDER BY m.band,m.clock,p.window_mcu_s''',
 'eligible_minutes':'''SELECT a.analysis_id,m.band,m.clock,p.start_mcu_s,max(p.reported_pk) AS maximum_individual_component_pk FROM pk_components p JOIN pk_analysis_runs a USING(analysis_id) JOIN pk_methods m USING(method_id) WHERE p.window_mcu_s=60 GROUP BY a.analysis_id,m.band,m.clock,p.start_mcu_s HAVING count(*)=3 AND sum(p.comparison_eligible)=3 ORDER BY m.band,m.clock,p.start_mcu_s''',
 'reference':'''SELECT definition_json FROM pk_reference''',
 'legacy_preserved':'''SELECT (SELECT count(*) FROM campaigns) AS campaigns,(SELECT count(*) FROM analysis_runs) AS previous_analyses,(SELECT count(*) FROM window_metrics) AS previous_rms_windows,(SELECT count(*) FROM pk_analysis_runs) AS pk_analyses'''
}

def query_summary(db):
    with ro(db) as c:
        c.row_factory=sqlite3.Row;result={}
        for name,sql in QUERIES.items():
            rows=[dict(r) for r in c.execute(sql)];result[name]=dict(count=len(rows),first_rows=rows[:3])
        return result

if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('results',type=Path);ap.add_argument('database',type=Path);ap.add_argument('--copy-v2',type=Path);a=ap.parse_args()
    if a.copy_v2:create_migrated_copy(a.copy_v2,a.database)
    r=import_results(a.results,a.database);r['queries']=query_summary(a.database);print(json.dumps(r,indent=2))
