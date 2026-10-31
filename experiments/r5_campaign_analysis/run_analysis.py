#!/usr/bin/env python3
"""R5 reproducible complete-file analysis. Creates a NEW PC output directory."""
import argparse, csv, gzip, hashlib, json, math, platform, resource, subprocess, sys, time
from pathlib import Path
import numpy as np
import scipy
from scipy import signal, fft, integrate
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from analysis import reader, verified_blocks, BandStream, Moments, design, velocity_spectral, DECIMATION, ROOT

EXPECTED = '91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5'
PAYLOAD = '5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41'
MISSION = 'TR2-20261031-R5-ANALYSE-PC-MACHINE-6H'
LIMITS = ('EXPLORATOIRE · mg nominaux · XYZ capteur · UTC inconnue · horloges non étalonnées\n'
          'Couplage mécanique non qualifié · aucune conclusion ISO/BV')


def sha(path):
    with Path(path).open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()


def save_json(path, data):
    path.write_text(json.dumps(data, indent=2, ensure_ascii=False, allow_nan=False)+'\n')


def save_csv(path, fields, rows):
    opener = gzip.open if path.suffix == '.gz' else open
    with opener(path, 'wt', newline='', encoding='utf-8') as f:
        w=csv.writer(f); w.writerow(fields); w.writerows(rows)


def windows(raw, band, duration, fs, width, out):
    r,b=raw.groups(width),band.groups(width)
    fields=['start_mcu_s','end_mcu_s','raw_n','band_n','raw_coverage_est','band_coverage_est','status']
    for name in ['raw','band']:
        fields += [name+'_'+k+'_mg' for k in ['rms_x','rms_y','rms_z','rms_global','peak_x','peak_y','peak_z','peak_vector']]
    rows=[]
    for i,start in enumerate(r['start_s']):
        end=min(float(start+width),duration+1/fs)
        expected=max(1.,(end-start)*fs)
        coverage=r['n'][i]/expected; valid=b['n'][i]/expected
        status='VALID_ESTIMATED' if (end-start >= width-.001 and .99 <= valid <= 1.01) else 'PARTIAL_OR_INVALID'
        row=[int(start),end,int(r['n'][i]),int(b['n'][i]),coverage,valid,status]
        for g in [r,b]: row += list(g['rms'][i])+[g['global_rms'][i]]+list(g['peak'][i])+[g['vector_peak'][i]]
        rows.append(row)
    save_csv(out/('windows_%02ds.csv'%width+('.gz' if width==1 else '')),fields,rows)
    return r,b,rows


def spectral_summary(data, fs, out):
    """Already band-limited derived samples. Batches bound FFT memory."""
    nperseg=2048; hop=1024; window=signal.windows.hann(nperseg,sym=False)
    starts=np.arange(0,len(data)-nperseg+1,hop)
    f=np.fft.rfftfreq(nperseg,1/fs); select=(f>=2)&(f<=25)
    selected=f[select]; times=(starts+nperseg/2)/fs
    psd=np.full((len(starts),int(select.sum()),3),np.nan,dtype=np.float32)
    scale=1/(fs*np.sum(window*window))
    for a in range(0,len(starts),32):
        subset=starts[a:a+32]
        x=np.array([data[s:s+nperseg] for s in subset],dtype=np.float64)
        valid=np.isfinite(x).all(axis=(1,2))
        if not valid.any(): continue
        z=np.fft.rfft(x[valid]*window[None,:,None],axis=1)
        p=abs(z)**2*scale;p[:,1:-1]*=2
        indices=a+np.flatnonzero(valid)
        psd[indices]=p[:,select]
    np.savez_compressed(out/'spectrogram.npz',time_mcu_s=times,frequency_est_Hz=selected,psd_xyz_mg2_per_Hz=psd)
    avg=np.nanmean(psd.sum(axis=2),axis=0)
    peaks,_=signal.find_peaks(avg,distance=max(1,int(1/(f[1]-f[0]))))
    peaks=sorted(peaks,key=lambda p:avg[p],reverse=True)[:3]
    candidates=sorted(float(selected[p]) for p in peaks)
    ridge_rows=[]
    for center in candidates:
        mask=abs(selected-center)<=.3
        last=None
        for i,t in enumerate(times):
            p=psd[i].sum(axis=1)
            if not np.isfinite(p).all():
                ridge_rows.append([t,center,'','','INVALID']);last=None;continue
            local=np.flatnonzero(mask); k=local[np.argmax(p[mask])]
            background=np.median(p)
            contrast=10*np.log10(max(p[k],1e-30)/max(background,1e-30))
            frequency=float(selected[k]);status='OBSERVED_FIXED_BAND'
            if contrast < 6: status='LOW_CONTRAST';frequency=np.nan
            elif last is not None and abs(frequency-last)>.2: status='DISCONTINUITY';frequency=np.nan
            ridge_rows.append([float(t),center,frequency,float(p[k]),status])
            last=frequency if np.isfinite(frequency) else None
    save_csv(out/'ridge_candidates.csv',['time_mcu_s','candidate_center_est_Hz','observed_est_Hz','peak_global_mg2_per_Hz','status'],ridge_rows)
    return dict(nperseg=nperseg,hop=hop,fs_est_Hz=fs,resolution_Hz=float(f[1]-f[0]),window_s=nperseg/fs,hop_s=hop/fs,window='periodic Hann',detrend=False,scaling='one-sided density; XYZ power SUM; mg²/Hz',dB_reference='1 mg²/Hz',invalid_frames=int((~np.isfinite(psd).all(axis=(1,2))).sum()),frames=len(starts),candidate_bands_Hz=[[c-.3,c+.3] for c in candidates]),times,selected,psd,ridge_rows


def selected_segments(path,info,starts):
    """One bounded second pass; reuses same accepted payload iterator."""
    pieces=[([],[]) for _ in starts]
    for _,t,x in verified_blocks(path,info):
        for start,(ts,xs) in zip(starts,pieces):
            mask=(t>=start-12)&(t<start+72)
            if mask.any():ts.append(t[mask]);xs.append(x[mask])
    return [(np.concatenate(ts),np.concatenate(xs)) for ts,xs in pieces]


def independent_checks(path,info,starts,raw60,band60,out):
    """FFT convolution against causal SOS streaming + Parseval for RAW.
    FFT evaluates SOS polynomials algebraically, NOT a second sosfilt call.
    Context >12s on both ends bounds circular convolution and startup effects.
    """
    fs=info['observed_output_hz_between_endpoints'];sos=design(fs)
    results=[];spectra=[]
    for label,start,(t,x) in zip(['low','median','high'],starts,selected_segments(path,info,starts)):
        take=(t>=start)&(t<start+60);nfft=fft.next_fast_len(len(x)+int(24*fs))
        f=np.fft.rfftfreq(nfft,1/fs);z=np.exp(-2j*np.pi*f/fs)
        h=np.ones(len(f),complex)
        for b0,b1,b2,a0,a1,a2 in sos:h*=(b0+b1*z+b2*z*z)/(a0+a1*z+a2*z*z)
        y=np.fft.irfft(np.fft.rfft(x,n=nfft,axis=0)*h[:,None],n=nfft,axis=0)[:len(x)]
        estimated=np.sqrt(np.mean(y[take]**2,axis=0))
        target=band60['rms'][int(start//60)]
        relative=np.abs(estimated-target)/np.maximum(target,1e-12)
        # Parseval independent raw RMS, correct one-sided weights.
        raw=x[take];p=abs(np.fft.rfft(raw,axis=0))**2
        weights=np.ones(len(p))*2;weights[0]=1
        if len(raw)%2==0:weights[-1]=1
        rms=np.sqrt(np.sum(p*weights[:,None],axis=0))/len(raw)
        target_raw=raw60['rms'][int(start//60)]
        raw_error=np.max(abs(rms-target_raw))
        if max(relative)>1e-3 or raw_error>1e-8: raise ValueError(f'independent cross-check failed {label}: {relative}, raw {raw_error}')
        # Segment spectrum of the independently filtered same real interval.
        f2,p2=signal.welch(y[take],fs=fs,window=signal.windows.hann(32768,sym=False),nperseg=32768,noverlap=16384,detrend=False,axis=0)
        choose=(f2>=2)&(f2<=25)
        spectra.append((label,start,f2[choose],p2[choose].sum(axis=1)))
        results.append(dict(label=label,start_mcu_s=int(start),duration_s=60,n=int(take.sum()),method='raw Parseval + FFT algebraic causal transfer / SOS streaming',band_rms_xyz_fft_mg=estimated.tolist(),band_rms_xyz_stream_mg=target.tolist(),relative_error_xyz=relative.tolist(),raw_parseval_max_abs_error_mg=float(raw_error),pass_limits=dict(relative_band=1e-3,absolute_raw_mg=1e-8),passed=True))
    save_json(out/'independent_checks.json',results)
    rows=[]
    for label,start,f,p in spectra:rows.extend([label,int(start),float(fi),float(pi)] for fi,pi in zip(f,p))
    save_csv(out/'segments_psd.csv',['segment','start_mcu_s','frequency_est_Hz','global_mg2_per_Hz'],rows)
    return results,spectra


def velocity_study(out):
    fs=4096;t=np.arange(int(8*fs))/fs;rows=[]
    for f in [1,2,5,10,20,40,80]:
        x=1000+10*np.sin(2*np.pi*f*t)
        v=velocity_spectral(x,fs)
        temporal=integrate.cumulative_trapezoid(x-x.mean(),dx=1/fs,initial=0)*9.80665
        temporal-=temporal.mean()
        expected=10*9.80665/(2*np.pi*f*np.sqrt(2))
        a=float(np.sqrt(np.mean(v*v)));b=float(np.sqrt(np.mean(temporal**2)))
        rows.append([f,10,1000,expected,a,b,a/expected-1,b/expected-1])
    save_csv(out/'velocity_synthetic.csv',['frequency_Hz','amplitude_peak_mg','offset_mg','theory_rms_mm_s','spectral_rms_mm_s','trapezoid_rms_mm_s','spectral_relative_error','trapezoid_relative_error'],rows)
    return dict(status='EXPLORATOIRE_NON_NORMATIF_SYNTHETIQUE',frequencies_Hz=[1,2,5,10,20,40,80],max_spectral_relative_error=max(abs(r[6]) for r in rows),max_trapezoid_relative_error=max(abs(r[7]) for r in rows),Wm_implemented=False,real_velocity_published=False)


def plots(out,r10,b10,r60,b60,times,freq,psd,spectra,ridges):
    plt.rcParams.update({'font.size':10,'axes.grid':True,'grid.alpha':.22,'figure.facecolor':'#f5f7fa','axes.spines.top':False,'axes.spines.right':False})
    colors=['#1479b8','#df7a17','#14956a']
    fig,ax=plt.subplots(3,1,figsize=(13,9),sharex=True,layout='constrained')
    for k,c in enumerate(colors):ax[0].plot((r60['start_s']+30)/3600,r60['rms'][:,k],label='XYZ'[k],color=c,lw=1)
    ax[0].set_ylabel('RMS brut (mg)');ax[0].set_title('Brut : composante statique conservée');ax[0].legend(ncol=3)
    for k,c in enumerate(colors):ax[1].plot((b60['start_s']+30)/3600,b60['rms'][:,k],label='XYZ'[k]+' · 60 s',color=c,lw=1)
    ax[1].plot((b10['start_s']+5)/3600,b10['global_rms'],color='#555f6b',alpha=.35,lw=.6,label='Global · 10 s')
    ax[1].plot((b60['start_s']+30)/3600,b60['global_rms'],color='#192a44',lw=1.4,label='Global · 60 s')
    ax[1].set_ylabel('RMS 2–25 Hz (mg)');ax[1].legend(ncol=3,fontsize=8)
    ax[2].plot((b10['start_s']+5)/3600,b10['vector_peak'],color='#8c3d85',lw=.8,label='Crête vectorielle réelle · 10 s')
    ax[2].set_ylabel('Crête 2–25 Hz (mg)');ax[2].set_xlabel('Temps relatif MCU nominal (h) — aucune UTC');ax[2].legend()
    fig.suptitle('E6N0005 · Intensité des vibrations sur toute la campagne\n'+LIMITS,fontsize=11)
    for axis in ax:
        axis.axvspan(0,60/3600,color='#9da4ad',alpha=.18)
        axis.axvspan(21960/3600,21999.988926/3600,color='#9da4ad',alpha=.18)
    ax[2].text(.01,.95,'Gris : fenêtres de bord incomplètes ; 5 s initiales exclues',transform=ax[2].transAxes,va='top',fontsize=8)
    fig.savefig(out/'01_intensity.png',dpi=150);plt.close(fig)
    power=psd.sum(axis=2).T
    db=10*np.log10(np.maximum(power,1e-12))
    lo,hi=np.nanpercentile(db,[5,99.5])
    fig,ax=plt.subplots(figsize=(13,5.5),layout='constrained')
    cmap=plt.get_cmap('magma').copy();cmap.set_bad('#aaaaaa')
    image=ax.pcolormesh(times/3600,freq,db,shading='nearest',cmap=cmap,vmin=lo,vmax=hi,rasterized=True)
    ax.set(ylim=(2,25),xlabel='Temps relatif MCU nominal (h) — première fenêtre invalide grisée',ylabel='Fréquence estimée (Hz)',title='E6N0005 · PSD globale = PSD X + Y + Z · bande filtrée 2–25 Hz\n'+LIMITS)
    fig.colorbar(image,ax=ax,label='10 log₁₀(PSD / [1 mg²/Hz])')
    fig.savefig(out/'02_spectrogram.png',dpi=150);plt.close(fig)
    fig,ax=plt.subplots(2,1,figsize=(13,8),layout='constrained')
    for label,start,f,p in spectra:ax[0].plot(f,10*np.log10(np.maximum(p,1e-12)),label=f'{label} : {start/3600:.3f}–{(start+60)/3600:.3f} h')
    ax[0].set(xlim=(2,25),xlabel='Fréquence estimée (Hz)',ylabel='PSD globale (dB réf. 1 mg²/Hz)',title='Trois minutes sélectionnées par intensité faible / médiane / forte')
    ax[0].legend()
    centers=sorted(set(r[1] for r in ridges))
    for center in centers:
        rows=[r for r in ridges if r[1]==center]
        y=np.array([float(r[2]) if r[2]!='' else np.nan for r in rows])
        ax[1].plot(np.array([r[0] for r in rows])/3600,y,lw=.7,label=f'Bande fixe {center:.2f} ± 0,30 Hz')
    ax[1].set(xlabel='Temps relatif MCU nominal (h)',ylabel='Maximum local estimé (Hz)',title='Raies candidates : bandes fixes, faible contraste et sauts masqués ; aucune source attribuée')
    ax[1].legend(ncol=3,fontsize=8)
    fig.suptitle('E6N0005 · Spectres et raies exploratoires\n'+LIMITS,fontsize=10)
    fig.savefig(out/'03_spectra_ridges.png',dpi=150);plt.close(fig)
    fig,ax=plt.subplots(2,1,figsize=(13,7),sharex=True,layout='constrained')
    for center in centers:
        rows=[r for r in ridges if r[1]==center]
        time_h=np.array([r[0] for r in rows])/3600
        accepted=np.array([r[4]=='OBSERVED_FIXED_BAND' for r in rows])
        observed=np.array([float(r[2]) if r[2]!='' else np.nan for r in rows])
        power=np.array([float(r[3]) if r[3]!='' else np.nan for r in rows])
        power[~accepted]=np.nan
        ax[0].plot(time_h,observed,lw=.65,label=f'{center:.2f} ± 0,30 Hz')
        ax[1].plot(time_h,10*np.log10(np.maximum(power,1e-12)),lw=.65)
    ax[0].set(ylabel='Maximum local estimé (Hz)',title='Bandes fixes candidates — interruptions conservées, aucune source attribuée')
    ax[0].legend(ncol=3)
    ax[1].set(ylabel='PSD au pic (dB réf. 1 mg²/Hz)',xlabel='Temps relatif MCU nominal (h)')
    fig.suptitle('E6N0005 · Fréquence et puissance des raies candidates\n'+LIMITS,fontsize=10)
    fig.savefig(out/'04_ridge_power.png',dpi=150);plt.close(fig)


def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('source',type=Path);ap.add_argument('output',type=Path);ap.add_argument('--qpc',type=Path)
    args=ap.parse_args();started=time.perf_counter();args.output.mkdir(parents=True,exist_ok=False);out=args.output
    print('G1 whole hash then strict reader',flush=True)
    whole=sha(args.source)
    if whole!=EXPECTED:raise ValueError('wrong source SHA')
    info=reader.verify(args.source)
    if info['status']!='COMPLETE' or info['sha256']!=PAYLOAD:raise ValueError('source not expected COMPLETE')
    save_json(out/'strict-import.json',info)
    fs=info['observed_output_hz_between_endpoints'];duration=info['observed_duration_us']*1e-6
    seconds=int(math.floor(duration))+1
    raw=Moments(seconds);band=Moments(seconds);filt=BandStream(fs)
    derived=np.memmap(out/'derived_band_xyz_mg.f32',dtype='float32',mode='w+',shape=((info['valid_xyz']+15)//16,3))
    max_clock_error=0.;max_dt=0.;last_t=None;raw_sum=np.zeros(3);total_n=0
    print('G3 complete bounded streaming pass',flush=True)
    for index,t,x in verified_blocks(args.source,info):
        valid=np.arange(index,index+len(x))>=11
        y=filt.apply(x,t,valid)
        raw.add(t,np.where(valid[:,None],x,np.nan));band.add(t,y)
        offset=(-index)%16
        k=(index+offset)//16
        derived[k:k+len(y[offset::16])]=y[offset::16]
        max_clock_error=max(max_clock_error,float(np.max(abs(t-np.arange(index,index+len(x))/fs))))
        if last_t is not None:max_dt=max(max_dt,float(t[0]-last_t))
        max_dt=max(max_dt,float(np.diff(t).max()));last_t=t[-1]
        raw_sum+=x[valid].sum(axis=0);total_n+=valid.sum()
    derived.flush()
    if filt.gaps:print(f'Observed {filt.gaps} time gaps; masked/reinitialized',flush=True)
    r1,b1,rows1=windows(raw,band,duration,fs,1,out)
    r10,b10,rows10=windows(raw,band,duration,fs,10,out)
    r60,b60,rows60=windows(raw,band,duration,fs,60,out)
    valid60=np.array([r[6]=='VALID_ESTIMATED' for r in rows60])
    values=b60['global_rms'][valid60];indices=np.flatnonzero(valid60);order=indices[np.argsort(values)]
    chosen=[int(b60['start_s'][i]) for i in [order[0],order[len(order)//2],order[-1]]]
    stats={}
    for width,g,rows in [(1,b1,rows1),(10,b10,rows10),(60,b60,rows60)]:
        valid=np.array([r[6]=='VALID_ESTIMATED' for r in rows]);v=g['global_rms'][valid]
        stats[str(width)]=dict(valid_full_windows=int(valid.sum()),total_windows=len(rows),rms_global_quantiles_mg=dict(zip(['p50','p90','p95','p99'],map(float,np.quantile(v,[.5,.9,.95,.99])))),max_window_rms_global_mg=float(v.max()),max_vector_peak_mg=float(np.nanmax(g['vector_peak'])))
    threshold=float(np.quantile(values,.9));elevated=valid60&(b60['global_rms']>=threshold)
    starts=np.flatnonzero(elevated&~np.r_[False,elevated[:-1]]);ends=np.flatnonzero(elevated&~np.r_[elevated[1:],False])
    save_csv(out/'elevated_episodes.csv',['start_mcu_s','end_mcu_s','max_60s_global_rms_mg','threshold_p90_mg','meaning'],[[int(b60['start_s'][a]),int(b60['start_s'][b]+60),float(b60['global_rms'][a:b+1].max()),threshold,'RELATIVE_DATA_THRESHOLD_NOT_COMFORT'] for a,b in zip(starts,ends)])
    print('Spectrogram and independent actual-data FFT cross-checks',flush=True)
    spectral,times,freq,psd,ridges=spectral_summary(derived,fs/16,out)
    checks,spectra=independent_checks(args.source,info,chosen,r60,b60,out)
    plots(out,r10,b10,r60,b60,times,freq,psd,spectra,ridges)
    velocity=velocity_study(out)
    test_f=np.array([0.,1.,2.,5.,10.,20.,25.,40.,80.,fs/16-25])
    h=signal.sosfreqz(design(fs),worN=test_f,fs=fs)[1]
    response=[dict(frequency_est_Hz=float(f),amplitude_gain=float(abs(v)),gain_dB=float(20*np.log10(abs(v))) if abs(v)>0 else None) for f,v in zip(test_f,h)]
    save_json(out/'filter.json',dict(method='Butterworth order4 prototype bandpass, total order8, causal SOS float64',fs_est_Hz=fs,sos=design(fs).tolist(),initialization='constant first valid XYZ; suppress 5s after each valid run restart',phase='causal, frequency-dependent delay; no zero-phase compensation',band_Hz=[2,25],edge_gain='-3.0103 dB at both edges, NOT a rectangular flat-band RMS',response=response,decimation=16,alias_scope='ONLY after 2–25Hz filtering; attenuation quantified at fs_decimated−25 Hz; no 2–700 claim'))
    qpc=None
    if args.qpc:
        spec_path=ROOT/'experiments/r3_time_reference/tools/analyze_reference.py'
        import importlib.util
        spec=importlib.util.spec_from_file_location('qpc_reader',spec_path);mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
        witness=json.loads(args.qpc.read_text(encoding='utf-8-sig'));qpc=mod.active_bounds(witness)
        if qpc['session']!=info['session']:raise ValueError('QPC/source session mismatch')
        a,b=witness['first'],witness['second'];fr=witness['frequency']
        low=(b['send_tick']-a['receive_tick'])/fr;high=(b['receive_tick']-a['send_tick'])/fr
        accepted=qpc['second']['accepted']-qpc['first']['accepted']
        qpc.update(path=str(args.qpc),sha256=sha(args.qpc),elapsed_s_bounds=[low,high],output_rate_est_Hz_bounds=[(accepted/10-1)/high,(accepted/10+1)/low],scope='same-session active snapshots; accepted count phase ±1 output; QPC not SI-traceable; not UTC')
    after=sha(args.source)
    if after!=whole:raise ValueError('source mutated during analysis')
    summary=dict(windows=stats,selected_segment_starts_mcu_s=chosen,relative_episode_threshold_p90_mg=threshold,episode_count=len(starts),raw_mean_xyz_mg=(raw_sum/total_n).tolist(),raw_n=int(raw.n.sum()),band_valid_n=int(band.n.sum()),excluded_fir_warmup_n=11,band_invalid_including_settling_n=int(info['valid_xyz']-band.n.sum()),timestamp_gaps_gt_10_periods=filt.gaps,max_adjacent_timestamp_interval_s=max_dt,max_uniform_grid_error_vs_archived_endpoints_s=max_clock_error)
    save_json(out/'summary.json',summary)
    save_json(out/'manifest.json',dict(mission_id=MISSION,source=info,whole_file_sha256=whole,whole_file_sha256_after=after,payload_sha256=info['sha256'],algorithm_code_sha256=hashlib.sha256(b''.join(Path(__file__).with_name(n).read_bytes() for n in ['analysis.py','run_analysis.py'])).hexdigest(),processing_git_sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),python=platform.python_version(),numpy=np.__version__,scipy=scipy.__version__,matplotlib=matplotlib.__version__,output_fs_est_Hz=fs,fs_method='sample endpoints against nominal MCU microseconds, NOT calibrated SI; uniform DSP approximation',header_initial_fs_est_Hz=info['output_hz'],QPC=qpc,UTC='UTC_UNKNOWN',site='plancher PC machine, double-face mince annoncé ; pose exploratoire',axes='XYZ capteur, orientation navire inconnue',quality='R3 CLOSED_WITH_RESERVATIONS; file/footer digest agrees internally, independent RAM digest lost after power cut',limits=LIMITS,spectrogram=spectral,velocity_study=velocity,performance=dict(elapsed_s=time.perf_counter()-started,peak_RSS_KiB=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss,method='resource.RUSAGE_SELF; Linux KiB includes mapped pages; bounded 4096-record batches, derived decimated file only')))
    files=[]
    for p in sorted(out.iterdir()):
        if p.is_file():files.append(dict(name=p.name,bytes=p.stat().st_size,sha256=sha(p)))
    save_json(out/'artifacts.json',files)
    print(json.dumps(dict(summary=summary,output=str(out),elapsed_s=time.perf_counter()-started,peak_RSS_KiB=resource.getrusage(resource.RUSAGE_SELF).ru_maxrss),indent=2),flush=True)

if __name__=='__main__':main()
