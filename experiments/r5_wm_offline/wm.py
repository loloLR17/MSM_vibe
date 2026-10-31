"""Original TR2-Wm calculation; local indicative metric, no instrument conformity."""
from pathlib import Path
import sys
import numpy as np
from scipy import signal
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'experiments/r5_prequal_offline'))
from prequal import Stream,Moments,reader,verified_blocks,old,sha,dump,canonical,clean,EXPECTED,PAYLOAD,pooled_rms,transfer
VERSION='TR2-Wm.v1';UNIT=9.80665
PARAMS=dict(f1_Hz=10**(-.1),f2_Hz=100.,f3_Hz=1/(.028*2*np.pi))
# Selected point oracles, not an exhaustive transcription of protected tables.
POINTS=[dict(nominal_Hz=1.,true_Hz=1.,rounded_gain=.833),dict(nominal_Hz=4.,true_Hz=10**.6,rounded_gain=.818),dict(nominal_Hz=10.,true_Hz=10.,rounded_gain=.494),dict(nominal_Hz=80.,true_Hz=10**1.9,rounded_gain=.0604)]

def analogue(f,variant='MAIN'):
 f=np.asarray(f);p=2j*np.pi*f;f1=PARAMS['f1_Hz']*({'LOW08':.8,'LOW12':1.2}.get(variant,1.))
 w1=2*np.pi*f1;w2=2*np.pi*PARAMS['f2_Hz'];w3=2*np.pi*PARAMS['f3_Hz']
 return (p*p/(p*p+np.sqrt(2)*w1*p+w1*w1))*(w2*w2/(p*p+np.sqrt(2)*w2*p+w2*w2))*(w3/(p+w3))

def design(fs,variant='MAIN'):
 if fs<=250:raise ValueError('fs must exceed 250Hz for this archived profile')
 f1=PARAMS['f1_Hz']*({'LOW08':.8,'LOW12':1.2}.get(variant,1.))
 # Band limitations included ONCE in Hh/Hl; no extra1–80 Butterworth.
 z1,p1,k1=signal.butter(2,2*np.pi*f1,btype='highpass',analog=True,output='zpk')
 z2,p2,k2=signal.butter(2,2*np.pi*100,btype='lowpass',analog=True,output='zpk')
 w3=2*np.pi*PARAMS['f3_Hz']
 z=np.r_[z1,z2];p=np.r_[p1,p2,-w3];k=k1*k2*w3
 zd,pd,kd=signal.bilinear_zpk(z,p,k,fs=fs)
 return signal.zpk2sos(zd,pd,kd)

def oracle_digital(f,fs,variant='MAIN'):
 # Independent complex rational evaluation, not a call to the SOS filter.
 return analogue(fs/np.pi*np.tan(np.pi*np.asarray(f)/fs),variant)

def definition(fs,clock,variant='MAIN'):
 sos=design(fs,variant)
 return dict(metric='TR2_WM_A_MAX' if variant=='MAIN' else 'TR2_WM_DIAGNOSTIC_'+variant,version=VERSION,variant=variant,unit='mm/s2',axis_rule='max of X/Y/Z RMS on same window; axis winner exported; NOT vector norm',parameters=PARAMS,sos=sos.tolist(),filter_sha256=__import__('hashlib').sha256(sos.astype('<f8').tobytes()).hexdigest(),clock=clock,fs_est_Hz=fs,window='aligned60/120 MCU seconds; 10ssettling +11FIRwarmups excluded; partial never FULL',useful_band_Hz=[1,80],band_policy='published smooth Hh/Hl included once, no extra brickwall; out-of-band leakage diagnostic separate',secondary_velocity='0.028 * acceleration_mm_s2; dependent equivalent, NOT independent physical integration',sensitivity_mg_count=.244,mg_to_mm_s2=UNIT,source_status='FS_PHYSIQUE_NON_VERIFIE_CONTINUOUS; nominal8g; uncalibrated',normative_compliance=False,local_category='NON_CLASSEE',BV_comparison='PENDING_LOT_B',orientation='sensor XYZ; ship axes unknown',time='UTC_UNKNOWN')

def metrics(y):
 if y.ndim!=2 or y.shape[1]!=3 or not len(y) or not np.isfinite(y).all():raise ValueError('three finite axes required')
 r=np.sqrt(np.mean(y*y,axis=0));i=int(np.argmax(r));return dict(rms_xyz=r,TR2_WM_A_MAX=float(r[i]),winning_axis='XYZ'[i],equivalent_v_mm_s=.028*float(r[i]))
