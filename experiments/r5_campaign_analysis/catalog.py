"""Isolated R5 SQLite catalogue. No dependency or access to the Windows service DB.
Import requires a completed analysis with the same immutable original whole SHA.
The strict source acceptance was done by run_analysis with the R3 versioned reader.
"""
import argparse, csv, gzip, hashlib, json, math, sqlite3, sys
from datetime import datetime, timezone
from pathlib import Path

APPLICATION_ID=1380270913
VERSION='r5.v1'
QUERIES={
 'campaigns': 'SELECT campaign_id,source_path,site,utc_status,duration_mcu_s FROM campaigns ORDER BY campaign_id',
 'global_per_minute': "SELECT start_mcu_s,rms_global,quality,coverage_est FROM window_metrics WHERE analysis_id=? AND indicator='band_2_25' AND window_s=60 ORDER BY start_mcu_s",
 'worst_10s': "SELECT start_mcu_s,rms_global,peak_vector,quality FROM window_metrics WHERE analysis_id=? AND indicator='band_2_25' AND window_s=10 AND quality='VALID_ESTIMATED' ORDER BY rms_global DESC,start_mcu_s LIMIT 1",
 'axes_600_660s': "SELECT start_mcu_s,rms_x,rms_y,rms_z,quality FROM window_metrics WHERE analysis_id=? AND indicator='band_2_25' AND window_s=1 AND start_mcu_s>=600 AND start_mcu_s<660 ORDER BY start_mcu_s"
}

def digest(path):
    with Path(path).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()

def canonical(data):return json.dumps(data,sort_keys=True,separators=(',',':'),allow_nan=False)

def number(v):
    x=float(v)
    return x if math.isfinite(x) else None

def import_results(directory, dbpath):
    """Transactionally import aggregate exports, verify original and export hashes.
    Idempotency keys include full method/parameter fingerprint and source SHA.
    Conflicting results under the same method are an error, never overwritten.
    """
    directory,dbpath=Path(directory).resolve(),Path(dbpath).resolve()
    manifest=json.loads((directory/'manifest.json').read_text())
    source=manifest['source'];sourcepath=Path(source['path'])
    if source['status']!='COMPLETE' or digest(sourcepath)!=manifest['whole_file_sha256']:
        raise ValueError('source missing/corrupt or not strictly COMPLETE')
    listed={v['name']:v for v in json.loads((directory/'artifacts.json').read_text())}
    csv_files=[directory/('windows_%02ds.csv'%w+('.gz' if w==1 else '')) for w in [1,10,60]]
    exported=['filter.json','spectrogram.npz','segments_psd.csv','ridge_candidates.csv']+[p.name for p in csv_files]
    for name in exported:
        if digest(directory/name)!=listed[name]['sha256']:raise ValueError('analysis artifact changed: '+name)
    filt=json.loads((directory/'filter.json').read_text())
    params=dict(filter=filt,sensitivity_mg_per_count=.244,time_windows='archived block interpolation / nominal MCU seconds',fir_warmup_outputs=11,algorithm_code_sha256=manifest['algorithm_code_sha256'])
    analysis_id=hashlib.sha256(canonical(dict(source=manifest['whole_file_sha256'],version=VERSION,parameters=params)).encode()).hexdigest()
    results_digest=hashlib.sha256(''.join(digest(p) for p in csv_files).encode()).hexdigest()
    # Refuse ALL existing databases without OUR application marker before any write.
    if dbpath.exists():
        with sqlite3.connect(dbpath.as_uri()+'?mode=ro',uri=True) as check:
            if check.execute('PRAGMA application_id').fetchone()[0]!=APPLICATION_ID or check.execute('PRAGMA user_version').fetchone()[0]!=1:
                raise ValueError('not an R5 schema1 catalogue; existing DB left untouched')
    dbpath.parent.mkdir(parents=True,exist_ok=True)
    connection=sqlite3.connect(dbpath)
    try:
        connection.execute('PRAGMA foreign_keys=ON');connection.execute('PRAGMA synchronous=FULL')
        if connection.execute('PRAGMA user_version').fetchone()[0]==0:
            connection.executescript('BEGIN IMMEDIATE;\n'+Path(__file__).with_name('schema_v1.sql').read_text()+'\nCOMMIT;')
        with connection:
            connection.execute('BEGIN IMMEDIATE')
            old=connection.execute('SELECT results_digest FROM analysis_runs WHERE analysis_id=?',(analysis_id,)).fetchone()
            if old:
                if old[0]!=results_digest:raise ValueError('same method, conflicting results; refusing overwrite')
                return dict(analysis_id=analysis_id,idempotent=True,counts=counts(connection))
            campaign=manifest['whole_file_sha256']
            connection.execute('INSERT OR IGNORE INTO campaigns VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)',
              (campaign,str(sourcepath),canonical(source['uid']),str(source['session']),source['format_version'],source['firmware_sha'],manifest['site'],manifest['UTC'],source['observed_duration_us']/1e6,manifest['output_fs_est_Hz'],.244,manifest['quality'],canonical(manifest)))
            connection.execute('INSERT INTO analysis_runs VALUES(?,?,?,?,?,?,?)',(analysis_id,campaign,'causal_SOS_RMS_PSD',VERSION,canonical(params),results_digest,'COMPLETE'))
            for width,path in zip([1,10,60],csv_files):
                opener=gzip.open if path.suffix=='.gz' else open
                with opener(path,'rt',newline='',encoding='utf-8') as f:
                    def rows():
                        for r in csv.DictReader(f):
                            for mode,name in [('raw','raw_DC'),('band','band_2_25')]:
                                coverage=float(r[mode+'_coverage_est']);n=int(r[mode+'_n'])
                                quality='VALID_ESTIMATED' if float(r['end_mcu_s'])-float(r['start_mcu_s'])>=width-.001 and .99<=coverage<=1.01 else 'PARTIAL_OR_INVALID'
                                yield (analysis_id,width,float(r['start_mcu_s']),float(r['end_mcu_s']),name,'mg',2 if mode=='band' else None,25 if mode=='band' else None,n,coverage,quality,*[number(r[mode+'_'+k+'_mg']) for k in ['rms_x','rms_y','rms_z','rms_global','peak_x','peak_y','peak_z','peak_vector']])
                    connection.executemany('INSERT INTO window_metrics VALUES('+','.join('?'*19)+')',rows())
            for name in exported:
                p=directory/name
                connection.execute('INSERT INTO artifact_refs VALUES(?,?,?,?,?)',(analysis_id,name,str(p),digest(p),p.stat().st_size))
        if connection.execute('PRAGMA integrity_check').fetchone()[0]!='ok' or connection.execute('PRAGMA foreign_key_check').fetchall():raise ValueError('SQLite integrity failure')
        return dict(analysis_id=analysis_id,idempotent=False,counts=counts(connection))
    finally:connection.close()

def counts(c):
    return {t:c.execute('SELECT count(*) FROM '+t).fetchone()[0] for t in ['campaigns','analysis_runs','window_metrics','artifact_refs']}

def query_summary(dbpath,analysis_id):
    result={}
    with sqlite3.connect(Path(dbpath).as_uri()+'?mode=ro',uri=True) as c:
        c.row_factory=sqlite3.Row
        for name,q in QUERIES.items():
            rows=[dict(r) for r in c.execute(q,() if name=='campaigns' else (analysis_id,))]
            result[name]=dict(count=len(rows),first_rows=rows[:3])
    return result

def run_import(directory,dbpath):
    """Errors recorded separately even if a transaction rolls back."""
    try:return import_results(directory,dbpath)
    except Exception as e:
        log=Path(str(dbpath)+'.errors.jsonl')
        log.parent.mkdir(parents=True,exist_ok=True)
        with log.open('a',encoding='utf-8') as f:f.write(canonical(dict(utc=datetime.now(timezone.utc).isoformat(),directory=str(directory),error_type=type(e).__name__,error=str(e)))+'\n')
        raise

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('analysis',type=Path);p.add_argument('database',type=Path);a=p.parse_args()
    try:r=run_import(a.analysis,a.database);r['queries']=query_summary(a.database,r['analysis_id']);print(json.dumps(r,indent=2))
    except Exception as e:print(json.dumps(dict(status='ERROR',error=str(e))),file=sys.stderr);raise SystemExit(1)
