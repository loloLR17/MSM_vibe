"""BV-PK exploratory component estimator, NOT a formal ISO6954 MRA instrument."""
from pathlib import Path
import sys
import numpy as np
from scipy import fft,signal,optimize
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'r5_campaign_analysis'))
from analysis import reader,verified_blocks
VERSION='pk-hann-idft-10s-60s.v1'
BANDS={'BV_PK_ACC_1_5':(1.,5.,'mm/s² peak'), 'BV_PK_VEL_5_100':(5.,100.,'mm/s peak')}
THRESHOLDS={'BV_PK_ACC_1_5':[125.,141.,157.], 'BV_PK_VEL_5_100':[4.,4.5,5.]}
CLOCK_FRAC=.00679
CONTRAST_DB=12.
SHAPE_LIMIT=.15


def hann_response(delta,n):
    """Exact DTFT of periodic Hann / its coherent sum; delta is in DFT bins."""
    delta=np.asarray(delta)
    def D(t):return n*np.exp(1j*np.pi*t*(n-1)/n)*np.sinc(t)/np.sinc(t/n)
    return (.5*D(delta)-.25*D(delta-1)-.25*D(delta+1))/(n/2)


def scalar(c,f,band):
    # 1mg =9.80665 mm/s² ; velocity is integrated ONE component at its frequency.
    return c*9.80665 if band=='BV_PK_ACC_1_5' else c*9.80665/(2*np.pi*f)


def spectrum(x):
    x=np.asarray(x,dtype=float)
    w=signal.windows.hann(len(x),sym=False)
    return fft.rfft((x-x.mean(axis=0))*w[:,None],axis=0),float(w.sum())


def candidates(X,cg,n,fs,axis,band,top=3,prune=True):
    low,high,_=BANDS[band];T=n/fs
    # Guard interval includes OUTSIDE peaks to avoid converting a sidelobe into a component.
    lo=max(2,int(np.floor(.5*T)));hi=min(len(X)-3,int(np.ceil(102*T)))
    mag=abs(X[:,axis]);peaks,_=signal.find_peaks(mag[lo:hi+1]);peaks=peaks+lo
    if not len(peaks):return []
    band_mask=(np.arange(len(mag))/T>=low)&(np.arange(len(mag))/T<=high)
    noise=max(float(np.median(mag[band_mask])),1e-20)
    selected=peaks[(peaks>=int(np.floor(low*T))-1)&(peaks<=int(np.ceil(high*T))+1)]
    coarse=2*mag[selected]/cg*9.80665
    if band=='BV_PK_VEL_5_100':coarse/=2*np.pi*(selected/T)
    order=np.argsort(coarse)[::-1]
    records=[]
    for idx in order:
        k=selected[idx]
        # Conservative bound: Hann correction<=1.179 and f-bin/f<=1.059 (k>=9).
        if prune and len(records)>=top and 1.3*coarse[idx]<sorted((r['component_pk'] for r in records),reverse=True)[top-1]:break
        # Hann interpolated-DFT identity (derived in METHOD.md), exact asymptotic single tone.
        den=2*X[k,axis]-X[k-1,axis]-X[k+1,axis]
        if abs(den)==0:continue
        delta=float(np.clip(2*np.real((X[k-1,axis]-X[k+1,axis])/den),-.5,.5))
        f=(k+delta)/T
        if not low-1e-8<=f<=high+1e-8:continue
        factor=abs(hann_response(delta,n));amp=float(2*mag[k]/cg/max(factor,1e-20))
        if amp<1e-6:continue # numerical floor only, NOT a measured sensor noise limit
        offsets=np.arange(-2,3);template=hann_response(delta-offsets,n)
        z=X[k+offsets,axis];coef=np.vdot(template,z)/np.vdot(template,template)
        shape=float(np.linalg.norm(z-coef*template)/max(np.linalg.norm(z),1e-20))
        contrast=float(20*np.log10(max(mag[k],1e-20)/noise))
        close=any(0<abs(j-k)<3 for j in peaks)
        # Symmetric nearby lines may be modulation sidebands OR distinct sources: refuse attribution.
        neighbors=[j for j in peaks if 0<abs(j-k)<=max(5,int(T)) and mag[j]>.08*mag[k]]
        sidebands=any(abs((a-k)+(b-k))<=1 for a in neighbors for b in neighbors if a<k<b)
        edge=min(abs(f-low),abs(f-high))<=CLOCK_FRAC*f+1/T
        status='SPECTRAL_CANDIDATE'
        if contrast<CONTRAST_DB:status='LOW_CONTRAST'
        elif close or shape>SHAPE_LIMIT:status='UNRESOLVED_OR_NON_SINUSOIDAL'
        elif sidebands:status='MODULATION_OR_MULTISOURCE'
        elif edge:status='BAND_EDGE_UNCERTAIN'
        records.append(dict(frequency_Hz=float(f),component_pk=scalar(amp,f,band),component_mg_pk=amp,psd_peak_mg2_per_Hz=float(2*mag[k]**2/(fs*3*n/8)),component_rms=scalar(amp,f,band)/np.sqrt(2),contrast_dB=contrast,shape_error=shape,bin_index=int(k),delta_bin=delta,bin_spacing_Hz=1/T,status=status,band_edge_uncertain=edge,sidebands_possible=sidebands))
    return sorted(records,key=lambda r:r['component_pk'],reverse=True)[:top]


def analyse_frame(t,x,start,width,fs,first_index,prepared=None):
    """No filter/transient invented: raw archive with mean subtraction, FIR warmup excluded by quality."""
    complete=t[-1]-start+1/fs>=width-.002
    invalid=None
    if first_index<11:invalid='INVALID_FIR_WARMUP'
    if not complete:invalid='PARTIAL_WINDOW'
    if not np.isfinite(x).all() or (abs(x)>=32767*.244).any():invalid='INVALID_CLIP_OR_NONFINITE'
    if len(t)<8 or np.any(np.diff(t)<=0) or np.any(np.diff(t)>10/fs):invalid='INVALID_TIME_GAP'
    X,cg=(prepared if prepared is not None else spectrum(x)) if invalid is None else (None,None)
    result={}
    for axis in range(3):
        for band in BANDS:
            found=candidates(X,cg,len(x),fs,axis,band) if X is not None else []
            result[axis,band]=dict(axis='XYZ'[axis],band=band,unit=BANDS[band][2],start_mcu_s=start,end_mcu_s=float(t[-1]),window_mcu_s=width,n=len(x),first_output_index=first_index,frame_status=invalid or 'COMPLETE',candidates=found)
    return result


def qualify_minute(main,short):
    """Match ONE candidate frequency, do not join changing dominant sources.
    Max short-window spectral peak is NOT claimed equal to ISO6954 MRA.
    """
    if not main['candidates']:return dict(status=main['frame_status'] if main['frame_status']!='COMPLETE' else 'NO_RESOLVED_COMPONENT',reported_pk=None,comparison_eligible=False)
    c=main['candidates'][0];out=dict(c,status=c['status'],reported_pk=c['component_pk'],comparison_eligible=False)
    if main['frame_status']!='COMPLETE':out['status']=main['frame_status'];return out
    if c['status']!='SPECTRAL_CANDIDATE':return out
    matches=[]
    for frame in short:
        possible=[q for q in frame['candidates'] if abs(q['frequency_Hz']-c['frequency_Hz'])<=.2]
        if frame['frame_status']!='COMPLETE' or not possible:
            out['status']='METHODOLOGY_INDETERMINATE_NO_MATCH';return out
        q=min(possible,key=lambda q:abs(q['frequency_Hz']-c['frequency_Hz']))
        if q['status']!='SPECTRAL_CANDIDATE':out['status']='METHODOLOGY_INDETERMINATE_SHORT_'+q['status'];return out
        matches.append(q)
    if len(matches)!=6:out['status']='PARTIAL_SUBWINDOW_COVERAGE';return out
    amps=np.array([q['component_pk'] for q in matches]);freq=np.array([q['frequency_Hz'] for q in matches]);med=float(np.median(amps))
    out.update(short_pk_min=float(amps.min()),short_pk_max=float(amps.max()),short_frequency_spread_Hz=float(np.ptp(freq)),median_short_vs_60_ratio=med/c['component_pk'])
    if np.ptp(freq)>.1 or amps.max()/max(amps.min(),1e-20)>1.25 or not .8<=med/c['component_pk']<=1.2:
        out['status']='METHODOLOGY_INDETERMINATE_MODULATED';return out
    strongest=matches[int(np.argmax(amps))]
    out.update(status='QUASI_STATIONARY_COMPONENT',reported_pk=float(amps.max()),reported_frequency_Hz=strongest['frequency_Hz'],reported_subwindow_start=short[int(np.argmax(amps))]['start_mcu_s'],comparison_eligible=True)
    return out


def compare(value,band,eligible):
    if not eligible or value is None:return 'INDETERMINATE'
    a,b,c=THRESHOLDS[band]
    for i,threshold in enumerate([a,b,c],1):
        if value==threshold:return f'AU_REPERE_{i}PK'
    if value<a:return 'SOUS_REPERE_1PK'
    if value<b:return 'ENTRE_1PK_2PK'
    if value<c:return 'ENTRE_2PK_3PK'
    return 'AU_DELA_3PK'


def temporal_component(x,fs,f0,band):
    """Independent rectangular time-domain variable-projection sinusoid fit.
    Frequency optimized, offset and linear drift fit jointly; no FFT amplitude used.
    """
    x=np.asarray(x);t=np.arange(len(x))/fs;center=t-t.mean();scale=max(t[-1],1e-9)
    def fit(f,return_coeff=False):
        a=np.column_stack([np.cos(2*np.pi*f*t),np.sin(2*np.pi*f*t),np.ones(len(t)),center/scale])
        coeff=np.linalg.lstsq(a,x,rcond=None)[0];res=x-a@coeff
        return coeff if return_coeff else float(res@res)
    delta=1/(len(x)/fs)
    result=optimize.minimize_scalar(fit,bounds=(max(.01,f0-delta),f0+delta),method='bounded',options={'xatol':1e-9})
    c=fit(result.x,True);amp=float(np.hypot(c[0],c[1]))
    return dict(frequency_Hz=float(result.x),component_mg_pk=amp,component_pk=scalar(amp,result.x,band),method='time-domain unweighted sinusoid+offset+linear-drift LS; frequency independently optimized',frequency_search_halfwidth_Hz=delta)
