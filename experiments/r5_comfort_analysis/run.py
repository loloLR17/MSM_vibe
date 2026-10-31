#!/usr/bin/env python3
"""Real PC-only NON NORMATIVE fallback while P0 Wm/ISO source remains unavailable."""
import argparse,csv,hashlib,json,math,re,resource,struct,subprocess,time
from pathlib import Path
import numpy as np
from scipy import signal,fft
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from core import filters,Stream,Moments,verified_blocks,reader,quality_mask,definition,canonical,compatible

ROOT=Path(__file__).resolve().parents[2]
EXPECTED='91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5'
PAYLOAD='5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41'
MISSION='TR2-20261031-R5-CONFORT-BV-ISO-INDICATIF'
LEGACY_DB=Path('/home/lolo/dev/msm/reports/TR2-R5-analysis-6h-20261031/r5-catalog-v1.sqlite')

def sha(p):
    with Path(p).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()

def dump(p,x):p.write_text(json.dumps(x,indent=2,ensure_ascii=False,allow_nan=False)+'\n')

def csvout(p,fields,rows):
    with p.open('w',newline='',encoding='utf-8') as f:
        w=csv.writer(f,lineterminator='\n');w.writerow(fields);w.writerows(rows)

def export_windows(moment,observed,duration,fs_mcu,width):
    g=moment.groups(width);starts=g['start_s'];obs=np.add.reduceat(observed,starts)
    rows=[]
    for i,start in enumerate(starts):
        end=min(start+width,duration+1/fs_mcu)
        full=end-start>=width-.001
        state='FULL' if full and obs[i]>0 and g['n'][i]==obs[i] else 'PARTIAL_OR_INVALID'
        rows.append(dict(window_mcu_s=width,start_mcu_s=int(start),end_mcu_s=float(end),observed_n=int(obs[i]),valid_n=int(g['n'][i]),coverage_observed=float(g['n'][i]/obs[i]) if obs[i] else 0.,quality=state,rms_x=float(g['rms'][i,0]),rms_y=float(g['rms'][i,1]),rms_z=float(g['rms'][i,2]),direction_max=float(np.max(g['rms'][i])),vector_technical=float(g['global_rms'][i]),peak_vector=float(g['vector_peak'][i])))
    return rows

def fir_response(info,clocks,out):
    p=ROOT/'Modbus RTU/05_Firmware/src/domain/acquisition/fir_decimator10_coefficients.h'
    q=list(map(int,re.findall(r'-?\d+',p.read_text().split('= {',1)[1].split('}',1)[0])))
    if len(q)!=107 or sha_bytes(struct.pack('<107h',*q))!=info['coefficient_sha256']:raise ValueError('archived FIR coefficient identity mismatch')
    rows=[]
    for clock,fs in clocks.items():
        for f in [0,.1,.25,1,2,5,10,20,40,80,fs-80]:
            h=np.sum(np.array(q)/32768*np.exp(-2j*np.pi*f/(10*fs)*np.arange(107)))
            rows.append([clock,f,abs(h),20*np.log10(abs(h)),53/(10*fs)])
    csvout(out/'fir_response.csv',['clock','estimated_Hz','amplitude_gain','gain_dB','group_delay_s'],rows)
    return dict(coefficient_sha256=info['coefficient_sha256'],header_file_sha256=sha(p),count=107,dc_sum_q15=sum(q),rounding_bound_mg_per_axis=.122,scope='linear response computed only; IIS3DWB/mounting/noise/clock physical response NOT calibrated; finite stopband, no alias-free claim')

def sha_bytes(x):return hashlib.sha256(x).hexdigest()

def checks_real(path,info,definitions,allrows,starts,out):
    pieces=[([],[]) for _ in starts]
    for _,t,x in verified_blocks(path,info):
        for start,(ts,xs) in zip(starts,pieces):
            m=(t>=start-24)&(t<start+84)
            if m.any():ts.append(t[m]);xs.append(x[m])
    checks=[]
    for start,(ts,xs) in zip(starts,pieces):
        t=np.concatenate(ts);x=np.concatenate(xs);take=(t>=start)&(t<start+60)
        nfft=fft.next_fast_len(len(x)+int(48*info['observed_output_hz_between_endpoints']))
        X=np.fft.rfft(x,n=nfft,axis=0)
        for clock in sorted(set(d['clock']['scenario'] for d in definitions.values())):
            ds={d['unit']:(mid,d) for mid,d in definitions.items() if d['clock']['scenario']==clock}
            fs=ds['mg'][1]['clock']['fs_est_Hz'];f=np.fft.rfftfreq(nfft,1/fs);z=np.exp(-2j*np.pi*f/fs)
            H=np.ones(len(f),complex)
            for b0,b1,b2,a0,a1,a2 in ds['mg'][1]['sos']:H*=(b0+b1*z+b2*z*z)/(a0+a1*z+a2*z*z)
            Ha=H.copy();Hv=np.zeros_like(H);Hp=np.zeros_like(H)
            positive=(f>0)&(f<fs/2)
            Hv[positive]=H[positive]*9.80665/(2j*fs*np.tan(np.pi*f[positive]/fs))
            Hp[positive]=H[positive]*9.80665/(2j*np.pi*f[positive])
            for unit,transfer in [('mg',Ha),('mm/s',Hv)]:
                mid,d=ds[unit];y=np.fft.irfft(X*transfer[:,None],n=nfft,axis=0)[:len(x)]
                rms=np.sqrt(np.mean(y[take]**2,axis=0))
                target=next(r for r in allrows if r['metric_definition_id']==mid and r['window_mcu_s']==60 and r['start_mcu_s']==start)
                r=np.array([target[k] for k in ['rms_x','rms_y','rms_z']]);err=abs(rms-r)/np.maximum(r,1e-12)
                if err.max()>1e-6:raise ValueError(f'independent algebraic FFT check {clock}/{unit} failed: {err}')
                record=dict(start_mcu_s=start,clock=clock,unit=unit,time_rms=r.tolist(),independent_fft_rms=rms.tolist(),relative_errors=err.tolist(),limit_relative=1e-6,passed=True)
                if unit=='mm/s':
                    yp=np.fft.irfft(X*Hp[:,None],n=nfft,axis=0)[:len(x)]
                    physical=np.sqrt(np.mean(yp[take]**2,axis=0));bias=(r/physical-1)
                    if abs(bias).max()>.01:raise ValueError('physical vs trapezoidal difference exceeds fixed1% preliminary limit')
                    record.update(physical_frequency_integration_rms=physical.tolist(),trapezoid_relative_bias=bias.tolist(),physical_control_limit=.01)
                checks.append(record)
    dump(out/'real_crosschecks.json',checks)
    return checks

def figures(rows,definitions,out):
    plt.rcParams.update({'font.size':10,'axes.grid':True,'grid.alpha':.2,'axes.spines.top':False,'axes.spines.right':False})
    fig,axes=plt.subplots(2,1,figsize=(12,8),sharex=True,layout='constrained')
    for ax,unit in zip(axes,['mg','mm/s']):
        for mid,d in definitions.items():
            if d['unit']!=unit:continue
            r=[r for r in rows if r['metric_definition_id']==mid and r['window_mcu_s']==60]
            x=np.array([r['start_mcu_s']+30 for r in r])/3600
            y=np.array([r['direction_max'] if r['quality']=='FULL' else np.nan for r in r])
            ax.plot(x,y,lw=1,label=d['clock']['scenario']+' · max RMS XYZ capteur · 60s')
            if d['clock']['scenario']=='MCU':
                for key,axis,c in [('rms_x','X','#2c81b5'),('rms_y','Y','#d98e1d'),('rms_z','Z','#218568')]:
                    ax.plot(x,[r[key] if r['quality']=='FULL' else np.nan for r in r],color=c,lw=.65,alpha=.5,label=axis+' · MCU')
                rr=[r for r in rows if r['metric_definition_id']==mid and r['window_mcu_s']==120]
                ax.plot(np.array([r['start_mcu_s']+60 for r in rr])/3600,[r['direction_max'] if r['quality']=='FULL' else np.nan for r in rr],color='black',ls='--',lw=.7,label='max XYZ · 120s MCU')
        ax.set_ylabel('RMS accélération (mg)' if unit=='mg' else 'RMS vitesse intégrée (mm/s)')
        ax.axvspan(0,120/3600,color='#999',alpha=.17);ax.legend(ncol=3,fontsize=8)
    axes[-1].set_xlabel('Temps relatif MCU nominal (h) — UTC inconnue')
    fig.suptitle('PC machine · indicateurs préliminaires NON PONDÉRÉS · Butterworth 1–80 Hz\nP0 bloquée : aucun Wm, équivalent ISO, seuil BV ou jugement de confort\nÉchelle/pose/axes/horloges non étalonnés · intégration trapézoïdale approximée',fontsize=11)
    fig.savefig(out/'unweighted_trends.png',dpi=150);plt.close(fig)

def main():
    a=argparse.ArgumentParser(description=__doc__);a.add_argument('source',type=Path);a.add_argument('output',type=Path);args=a.parse_args();args.output.mkdir(parents=True,exist_ok=False);out=args.output;started=time.perf_counter()
    if sha(args.source)!=EXPECTED:raise ValueError('source SHA mismatch')
    info=reader.verify(args.source)
    if info['status']!='COMPLETE' or info['sha256']!=PAYLOAD:raise ValueError('source not expected COMPLETE')
    # Witness/provenance accepted in the previous mission, read without rerunning it.
    witness_path=Path('/home/lolo/dev/msm/reports/TR2-R5-analysis-6h-20261031/full-analysis-v1/manifest.json')
    expected_witness=json.loads((ROOT/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/manifest.json').read_text())
    witness=json.loads(witness_path.read_text());assert witness==expected_witness
    qpc=witness['QPC'];assert sha(qpc['path'])==qpc['sha256']
    clocks=dict(MCU=info['observed_output_hz_between_endpoints'],QPC_ESTIMATE=sum(qpc['output_rate_est_Hz_bounds'])/2)
    duration=info['observed_duration_us']/1e6;seconds=int(math.floor(duration))+1
    definitions={};streams={};moments={}
    for clock,fs in clocks.items():
        for unit,sos in zip(['mg','mm/s'],filters(fs)):
            d=definition(fs,clock,unit);mid=sha_bytes(canonical(d).encode());definitions[mid]=d;streams[mid]=Stream(fs,sos);moments[mid]=Moments(seconds)
    observed=np.zeros(seconds,np.int64)
    print('strict import COMPLETE; full original XYZ -> four isolated unweighted streams',flush=True)
    for index,t,x in verified_blocks(args.source,info):
        observed+=np.bincount(np.floor(t).astype(int),minlength=seconds)
        valid=quality_mask(x,np.arange(index,index+len(x)))
        for mid,stream in streams.items():moments[mid].add(t,stream.apply(x,t,valid))
    rows=[]
    for mid,m in moments.items():
        for width in [60,120]:
            for r in export_windows(m,observed,duration,clocks['MCU'],width):r.update(metric_definition_id=mid,unit=definitions[mid]['unit'],clock=definitions[mid]['clock']['scenario']);rows.append(r)
    fields=['metric_definition_id','unit','clock','window_mcu_s','start_mcu_s','end_mcu_s','observed_n','valid_n','coverage_observed','quality','rms_x','rms_y','rms_z','direction_max','vector_technical','peak_vector']
    csvout(out/'metrics.csv',fields,[[r[k] for k in fields] for r in rows])
    summary=[]
    for mid,d in definitions.items():
        for width in [60,120]:
            valid=[r for r in rows if r['metric_definition_id']==mid and r['window_mcu_s']==width and r['quality']=='FULL'];v=np.array([r['direction_max'] for r in valid])
            summary.append(dict(metric_definition_id=mid,unit=d['unit'],clock=d['clock']['scenario'],window_mcu_s=width,full_windows=len(v),P50=float(np.median(v)),P95=float(np.quantile(v,.95)),maximum=float(max(v)),population_std=float(np.std(v)),selected_direction='max RMS among sensor XYZ; NOT ship-axis normative selection',threshold_comparison=None))
    base=[r for r in rows if r['clock']=='MCU' and r['unit']=='mm/s' and r['window_mcu_s']==60 and r['quality']=='FULL'];ordered=sorted(base,key=lambda r:r['direction_max']);starts=[r['start_mcu_s'] for r in [ordered[0],ordered[len(ordered)//2],ordered[-1]]]
    checks=checks_real(args.source,info,definitions,rows,starts,out)
    sensitivity=[]
    for r in rows:
        if r['clock']!='MCU' or r['quality']!='FULL':continue
        q=next(v for v in rows if v['clock']=='QPC_ESTIMATE' and v['unit']==r['unit'] and v['start_mcu_s']==r['start_mcu_s'] and v['window_mcu_s']==r['window_mcu_s'])
        if q['quality']=='FULL':sensitivity.append([r['unit'],r['window_mcu_s'],r['start_mcu_s'],r['direction_max'],q['direction_max'],100*(q['direction_max']/r['direction_max']-1)])
    csvout(out/'clock_sensitivity.csv',['unit','window_mcu_s','start_mcu_s','MCU_direction_max','QPC_direction_max','relative_difference_percent'],sensitivity)
    fir=fir_response(info,clocks,out)
    figures(rows,definitions,out)
    after=sha(args.source)
    if after!=EXPECTED:raise ValueError('source changed')
    dump(out/'definitions.json',definitions);dump(out/'summary.json',summary)
    response=[]
    for clock,fs in clocks.items():
        for f in [.1,.25,1,2,5,10,20,40,80,160]:
            ha,hv=[signal.sosfreqz(s,worN=[f],fs=fs)[1][0] for s in filters(fs)]
            response.append(dict(clock=clock,frequency_Hz=f,acceleration_gain=float(abs(ha)),velocity_gain_mm_s_per_mg=float(abs(hv)),velocity_phase_minus_accel_rad=float(np.angle(hv/ha)),trapezoid_vs_ideal_ratio=float((np.pi*f/fs)/np.tan(np.pi*f/fs))))
    dump(out/'response.json',response)
    manifest=dict(mission_id=MISSION,status='PRELIMINARY_P0_BLOCKED',normative_names_enabled=False,normative_thresholds_enabled=False,source=info,whole_file_sha256=EXPECTED,whole_file_sha256_after=after,processing_git_sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),algorithm_code_sha256=sha(Path(__file__)),core_code_sha256=sha(Path(__file__).with_name('core.py')),clocks=clocks,QPC_scope=qpc['scope'],time='UTC_UNKNOWN; windows use SAME archived nominal MCU intervals in both clock scenarios; QPC changes frequency/integration interpretation ONLY',FIR=fir,selected_real_segment_starts_mcu_s=starts,legacy_database=dict(path=str(LEGACY_DB),sha256=sha(LEGACY_DB)),legacy_results_preserved=True,comparability={mid:compatible(d,d) for mid,d in definitions.items()},performance=dict(elapsed_s=time.perf_counter()-started,peak_RSS_KiB=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss),limits=['NO Wm/ISO equivalent velocity/BV comparisons','sensor axes unknown, sensitivity .244mg/count nominal, mounting/clock uncalibrated','R3 independent RAM producer digest lost; internal integrity only','Butterworth1–80Hz -3dB edges, finite rolloff; not ideal brickwall','stable trapezoidal physical integration: tan frequency warping quantified separately, not normative equivalence','10s settling invalid; all observed points required for FULL; partial/empty -> NaN, never zero filled'])
    dump(out/'manifest.json',manifest)
    dump(out/'artifacts.json',[dict(name=p.name,bytes=p.stat().st_size,sha256=sha(p)) for p in sorted(out.iterdir()) if p.is_file()])
    print(json.dumps(dict(output=str(out),summary=summary,performance=manifest['performance']),indent=2),flush=True)

if __name__=='__main__':main()
