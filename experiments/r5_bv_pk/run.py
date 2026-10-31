#!/usr/bin/env python3
"""Full original .TR2 -> component peaks, no previous RMS recomputation."""
import argparse,csv,gzip,hashlib,io,json,math,re,resource,struct,subprocess,time
from pathlib import Path
from collections import Counter
import numpy as np
from core import *
ROOT=Path(__file__).resolve().parents[2]
EXPECTED='91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5'
PAYLOAD='5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41'
MISSION='TR2-20261031-R5-BV-PK-ALTERNATIF-6H'

def sha(p):
    with Path(p).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()

def json_scalar(x):
    if isinstance(x,np.generic):return x.item()
    raise TypeError(f'Unsupported JSON value {type(x).__name__}')

def dump(p,x):p.write_text(json.dumps(x,ensure_ascii=False,indent=2,allow_nan=False,default=json_scalar)+'\n')

def minutes(path,info):
    ts=[];xs=[];current=0;first_index=0
    for index,t,x in verified_blocks(path,info):
        ids=np.floor(t/60).astype(int);starts=np.r_[0,np.flatnonzero(np.diff(ids))+1,len(t)]
        for a,b in zip(starts[:-1],starts[1:]):
            mid=int(ids[a])
            if mid!=current:
                yield current*60,np.concatenate(ts),np.concatenate(xs),first_index
                ts=[];xs=[];current=mid;first_index=index+int(a)
            ts.append(t[a:b]);xs.append(x[a:b])
    if ts:yield current*60,np.concatenate(ts),np.concatenate(xs),first_index

FIELDS=['method_id','clock','band','unit','axis','start_mcu_s','end_mcu_s','window_mcu_s','sample_n','valid_input_n','frame_status','status','frequency_Hz','component_pk','component_rms','reported_pk','reported_frequency_Hz','reported_subwindow_start','component_mg_pk','psd_peak_mg2_per_Hz','fixed_frequency_rounding_bound_pk','contrast_dB','shape_error','bin_spacing_Hz','short_pk_min','short_pk_max','short_frequency_spread_Hz','median_short_vs_60_ratio','comparison_eligible','comparison_zone']

def row(frame,clock,method_id,q=None):
    c=frame['candidates'][0] if frame['candidates'] else {}
    if q is None:q=dict(c,status=c.get('status',frame['frame_status'] if frame['frame_status']!='COMPLETE' else 'NO_RESOLVED_COMPONENT'),comparison_eligible=False,reported_pk=c.get('component_pk'))
    value=q.get('reported_pk');eligible=q.get('comparison_eligible',False)
    # A method spread of5% close to a reference must not become a confident category.
    if eligible and any(abs(value-threshold)<=.05*threshold for threshold in THRESHOLDS[frame['band']]):
        eligible=False;q=dict(q,status='METHODOLOGY_INDETERMINATE_NEAR_REFERENCE')
    out={k:None for k in FIELDS};out.update({k:v for k,v in frame.items() if k in FIELDS})
    out.update({k:v for k,v in q.items() if k in FIELDS})
    out.update(method_id=method_id,clock=clock,sample_n=frame['n'],valid_input_n=max(0,frame['n']-max(0,11-frame['first_output_index'])),comparison_eligible=eligible,comparison_zone=compare(value,frame['band'],eligible))
    return out


def independent(path,info,clocks,allrows,out):
    # Three measured low/median/high minutes for each band, choose actual strongest axis.
    choices=[]
    for band in BANDS:
        grouped={}
        for r in allrows:
            if r['clock']=='MCU' and r['band']==band and r['window_mcu_s']==60 and r['component_pk'] is not None and r['frame_status']=='COMPLETE':
                s=r['start_mcu_s']
                if s not in grouped or r['component_pk']>grouped[s]['component_pk']:grouped[s]=r
        robust=[r for r in grouped.values() if r['comparison_eligible']]
        selection_pool=robust if len(robust)>=3 else list(grouped.values())
        ordered=sorted(selection_pool,key=lambda r:r['component_pk'])
        for label,r in zip(['low','median','high'],[ordered[0],ordered[len(ordered)//2],ordered[-1]]):choices.append((label,r))
    needed={r['start_mcu_s'] for _,r in choices};pieces={}
    for start,t,x,idx in minutes(path,info):
        if start in needed:pieces[start]=(t,x)
    checks=[]
    for label,r in choices:
        for clock,fs in clocks.items():
            q=next(q for q in allrows if q['clock']==clock and q['band']==r['band'] and q['axis']==r['axis'] and q['start_mcu_s']==r['start_mcu_s'] and q['window_mcu_s']==60)
            tt,xx=pieces[r['start_mcu_s']];x=xx[:,'XYZ'.index(r['axis'])]
            if q['frequency_Hz'] is None:continue
            ls=temporal_component(x,fs,q['frequency_Hz'],q['band'])
            amp_error=abs(ls['component_pk']/q['component_pk']-1);freq_error=abs(ls['frequency_Hz']-q['frequency_Hz'])
            agree=amp_error<=.05 and freq_error<=.02
            check=dict(selection=label,start_mcu_s=r['start_mcu_s'],band=r['band'],clock=clock,axis=q['axis'],spectral_pk=q['component_pk'],spectral_frequency_Hz=q['frequency_Hz'],temporal=ls,relative_amplitude_discrepancy=amp_error,frequency_discrepancy_Hz=freq_error,limits=dict(relative_amplitude=.05,frequency_Hz=.02),agreement=agree,initial_status=q['status'])
            if q.get('reported_subwindow_start') is not None:
                ss=q['reported_subwindow_start'];take=(tt>=ss)&(tt<ss+10)
                short_ls=temporal_component(x[take],fs,q['reported_frequency_Hz'],q['band'])
                short_error=abs(short_ls['component_pk']/q['reported_pk']-1)
                check.update(short_window_start_mcu_s=ss,short_temporal=short_ls,short_relative_amplitude_discrepancy=short_error)
                agree=agree and short_error<=.05
                check['agreement']=agree
            if q['comparison_eligible'] and not agree:
                q.update(comparison_eligible=False,comparison_zone='INDETERMINATE',status='METHODOLOGY_INDETERMINATE_TIME_FIT')
            checks.append(check)
    failed={(c['clock'],c['band']) for c in checks if c['initial_status']=='QUASI_STATIONARY_COMPONENT' and not c['agreement']}
    for q in allrows:
        if q['window_mcu_s']==60 and (q['clock'],q['band']) in failed:
            q.update(comparison_eligible=False,comparison_zone='INDETERMINATE',status='METHODOLOGY_INDETERMINATE_CONTROL_AUDIT')
    dump(out/'independent_real.json',checks)
    return checks


def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('source',type=Path);ap.add_argument('output',type=Path);ap.add_argument('--resume-spectral',type=Path);a=ap.parse_args();a.output.mkdir(parents=True,exist_ok=False);out=a.output;started=time.perf_counter()
    if sha(a.source)!=EXPECTED:raise ValueError('source whole SHA')
    info=reader.verify(a.source)
    if info['status']!='COMPLETE' or info['sha256']!=PAYLOAD:raise ValueError('not expected COMPLETE')
    previous=json.loads((ROOT/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF/manifest.json').read_text());clocks=previous['clocks']
    assert clocks['MCU']==info['observed_output_hz_between_endpoints']
    methods={};ids={}
    for clock,fs in clocks.items():
        for band in BANDS:
            method=dict(edition='BV_NR467_F_2026_07_PK',derived_method_version=VERSION,clock=clock,fs_est_Hz=fs,band=band,range_Hz=list(BANDS[band][:2]),unit=BANDS[band][2],CF=1,window_mcu_s=[10,60],mean_subtracted=True,Hann='periodic; coherent sum correction and exact DTFT',frequency_estimator='three-complex-bin Hann interpolation',selection='max individual component; max6 matched10s in a quasi-stationary60s frame; NOT ISO MRA',contrast_min_dB=12,shape_limit=.15,frequency_spread_max_Hz=.1,amplitude_ratio_max=1.25,clock_fraction_for_bandedge=CLOCK_FRAC,near_reference_method_margin=.05,sensitivity_mg_per_count=.244,axis='XYZ sensor, ship orientation unknown',site='PC machine plancher, continuous occupation unconfirmed',mount='double-face experimental, uncalibrated',UTC='UTC_UNKNOWN',formal_ISO6954_compliance=False,reference_document_sha256='5ae75d30f6b38a1331431e16bcc3dce11bc42c90b393f344be89b42cb050ceef',code_sha256=sha(Path(__file__).with_name('core.py')))
            mid=hashlib.sha256(json.dumps(method,sort_keys=True,separators=(',',':')).encode()).hexdigest();methods[mid]=method;ids[clock,band]=mid
    allrows=[]
    print('Strict COMPLETE; streaming 60s +6x10s spectral components, two clock interpretations',flush=True)
    if a.resume_spectral:
        with gzip.open(a.resume_spectral,'rt') as f:checkpoint=json.load(f)
        if checkpoint['source_sha256']!=EXPECTED or checkpoint['core_sha256']!=sha(Path(__file__).with_name('core.py')):raise ValueError('spectral checkpoint identity')
        if checkpoint['methods']!=methods:raise ValueError('spectral method changed')
        allrows=checkpoint['rows']
        print('resumed verified aggregate spectral checkpoint, no FFT extraction repeated',flush=True)
    else:
        for start,t,x,index in minutes(a.source,info):
            # Mean/Hann transform computed once per frame, reused for the two clock hypotheses.
            frames=[(start,60,t,x,index)]
            for k in range(6):
                m=(t>=start+10*k)&(t<start+10*(k+1))
                if m.any():frames.append((start+10*k,10,t[m],x[m],index+int(np.flatnonzero(m)[0])))
            prepared=[spectrum(xx) if np.isfinite(xx).all() else None for _,_,_,xx,_ in frames]
            for clock,fs in clocks.items():
                found=[]
                for frame_idx,(s,w,tt,xx,idx) in enumerate(frames):found.append(analyse_frame(tt,xx,s,w,fs,idx,prepared=prepared[frame_idx]))
                for axis in range(3):
                    for band in BANDS:
                        mainframe=found[0][axis,band];short=[ff[axis,band] for ff in found[1:]]
                        q=qualify_minute(mainframe,short);allrows.append(row(mainframe,clock,ids[clock,band],q))
                        for f in short:allrows.append(row(f,clock,ids[clock,band]))
            if start%3600==0:print('processed through nominal MCU hour',start/3600,flush=True)
        checkpoint=dict(source_sha256=EXPECTED,core_sha256=sha(Path(__file__).with_name('core.py')),methods=methods,rows=allrows)
        with gzip.open(out/'spectral_checkpoint.json.gz','wt',encoding='utf-8') as f:
            json.dump(checkpoint,f,allow_nan=False,default=json_scalar)
        print('spectral aggregate checkpoint saved before independent controls/export',flush=True)
    controls=independent(a.source,info,clocks,allrows,out)
    # One gzip, deterministic container header, only aggregate component rows.
    with (out/'components.csv.gz').open('wb') as binary:
        with gzip.GzipFile(filename='',mode='wb',fileobj=binary,mtime=0) as z:
            with io.TextIOWrapper(z,encoding='utf-8',newline='') as text:
                w=csv.DictWriter(text,fieldnames=FIELDS,lineterminator='\n');w.writeheader();w.writerows(allrows)
    summary=[]
    for clock in clocks:
        for band in BANDS:
            rows=[r for r in allrows if r['clock']==clock and r['band']==band and r['window_mcu_s']==60]
            # Whole-minute result requires ALL three axes compatible, not a smaller valid subset.
            minutes_by={}
            for r in rows:minutes_by.setdefault(r['start_mcu_s'],[]).append(r)
            eligible=[max(rs,key=lambda r:r['reported_pk']) for rs in minutes_by.values() if len(rs)==3 and all(r['comparison_eligible'] for r in rs)]
            values=np.array([r['reported_pk'] for r in eligible])
            allcandidate=[max([r['reported_pk'] for r in rs if r['reported_pk'] is not None],default=np.nan) for rs in minutes_by.values()]
            summary.append(dict(clock=clock,band=band,unit=BANDS[band][2],total_minutes=len(minutes_by),comparable_all_axes_minutes=len(eligible),candidate_finite_minutes=int(np.isfinite(allcandidate).sum()),eligible_axis_windows=sum(bool(r['comparison_eligible']) for r in rows),by_axis={axis:dict(eligible=sum(bool(r['comparison_eligible']) for r in rows if r['axis']==axis),total=sum(r['axis']==axis for r in rows)) for axis in 'XYZ'},eligible_peak_P50=float(np.median(values)) if len(values) else None,eligible_peak_P95=float(np.quantile(values,.95)) if len(values) else None,eligible_peak_max=float(values.max()) if len(values) else None,axis_status_counts=dict(Counter(r['status'] for r in rows)),indicative_minute_zone_counts=dict(Counter(compare(r['reported_pk'],band,True) for r in eligible))))
    coeffpath=ROOT/'Modbus RTU/05_Firmware/src/domain/acquisition/fir_decimator10_coefficients.h';q=list(map(int,re.findall(r'-?\d+',coeffpath.read_text().split('= {',1)[1].split('}',1)[0])))
    assert hashlib.sha256(struct.pack('<107h',*q)).hexdigest()==info['coefficient_sha256']
    response=[]
    for clock,fs in clocks.items():
        for f in [1,2,5,10,20,40,80,99,100,fs-100]:
            h=np.sum(np.array(q)/32768*np.exp(-2j*np.pi*f/(fs*10)*np.arange(107)));response.append(dict(clock=clock,frequency_est_Hz=f,gain=float(abs(h)),gain_dB=float(20*np.log10(abs(h))),delay_s=53/(fs*10)))
    dump(out/'fir_response.json',response);dump(out/'methods.json',methods);dump(out/'summary.json',summary)
    after=sha(a.source)
    if after!=EXPECTED:raise ValueError('original changed')
    manifest=dict(mission_id=MISSION,source=info,whole_file_sha256=EXPECTED,whole_file_sha256_after=after,processing_git_sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),runner_sha256=sha(Path(__file__)),core_sha256=sha(Path(__file__).with_name('core.py')),clocks=clocks,component_rows=len(allrows),sample_n=info['valid_xyz'],methods=list(methods),quality='exploratory, uncalibrated; orientation/mount unknown; RAM producer digest lost, internal integrity only',time='same archived MCU windows, no UTC/SI correction',references=dict(velocity=THRESHOLDS['BV_PK_VEL_5_100'],acceleration=THRESHOLDS['BV_PK_ACC_1_5'],label='Référence BV : PC machine occupé en permanence, correspondance à confirmer',formal_grade_awarded=False),Wm_status='BLOCKED_UNCHANGED_SEPARATE_MISSION',performance=dict(elapsed_s=time.perf_counter()-started,peak_RSS_KiB=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss))
    dump(out/'manifest.json',manifest);dump(out/'artifacts.json',[dict(name=p.name,bytes=p.stat().st_size,sha256=sha(p)) for p in sorted(out.iterdir()) if p.is_file()])
    print(json.dumps(dict(summary=summary,performance=manifest['performance']),indent=2),flush=True)

if __name__=='__main__':main()
