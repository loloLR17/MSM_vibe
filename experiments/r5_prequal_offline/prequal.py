"""Offline numerical feasibility only; no normative comfort weighting or device I/O."""
from pathlib import Path
import sys, importlib.util, hashlib, json
import numpy as np
from scipy import signal
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'experiments/r5_campaign_analysis'))
from analysis import verified_blocks, reader, Moments
spec=importlib.util.spec_from_file_location('r5_existing_unweighted',ROOT/'experiments/r5_comfort_analysis/core.py')
old=importlib.util.module_from_spec(spec);spec.loader.exec_module(old)
Stream=old.Stream
VERSION='prequal-global-energy-v1'
BANDS={'BAND_1_2':(1.,2.),'BAND_2_5':(2.,5.),'BAND_5_25':(5.,25.),'BAND_25_80':(25.,80.),'FLAT_1_80':(1.,80.),'TEST_LP10':(1.,80.),'EDGE_08':(.8,80.),'EDGE_12':(1.2,80.)}
EXPECTED='91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5'
PAYLOAD='5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41'
SOURCE=Path('/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2')

def sha(p):
 with Path(p).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def clean(v):
 if isinstance(v,dict):return {k:clean(x) for k,x in v.items()}
 if isinstance(v,(list,tuple)):return [clean(x) for x in v]
 if isinstance(v,np.ndarray):return clean(v.tolist())
 if isinstance(v,np.generic):return clean(v.item())
 if isinstance(v,float) and not np.isfinite(v):return None
 return v
def dump(p,v):Path(p).write_text(json.dumps(clean(v),indent=2,ensure_ascii=False,allow_nan=False)+'\n')
def canonical(v):return json.dumps(clean(v),sort_keys=True,separators=(',',':'),allow_nan=False)
def design(fs,name):
 lo,hi=BANDS[name];sos=signal.butter(4,[lo,hi],btype='bandpass',fs=fs,output='sos')
 if name=='TEST_LP10':
  # Explicit test template, H(s)=1/(1+s/(2*pi*10)), bilinear mapping.
  b,a=signal.bilinear([2*np.pi*10],[1,2*np.pi*10],fs=fs)
  sos=np.vstack([sos,signal.tf2sos(b,a)])
 return sos

def transfer(sos,f,fs):
 z=np.exp(-2j*np.pi*np.asarray(f)/fs);h=np.ones(z.shape,complex)
 for b0,b1,b2,a0,a1,a2 in sos:h*=(b0+b1*z+b2*z*z)/(a0+a1*z+a2*z*z)
 return h

def oracle_gain(f,fs,name):
 """Closed form analogue Butterworth bandpass + prewarping, independent of SOS."""
 lo,hi=BANDS[name];L,H=2*fs*np.tan(np.pi*np.array([lo,hi])/fs)
 w=2*fs*np.tan(np.pi*np.asarray(f)/fs)
 q=(w*w-L*H)/((H-L)*w)
 amp=1/np.sqrt(1+q**8)
 if name=='TEST_LP10':amp/=np.sqrt(1+(w/(2*np.pi*10))**2)
 return amp

def definition(fs,name,clock):
 sos=design(fs,name)
 return dict(version=VERSION,name=name,unit='mg RMS nominal',normative=False,template='flat' if name!='TEST_LP10' else 'sensitivity-test RC lowpass10Hz; NOT a comfort model',band=BANDS[name],sos=sos.tolist(),filter_sha256=hashlib.sha256(sos.astype('<f8').tobytes()).hexdigest(),clock=clock,fs_est_Hz=fs,physical_clock_verified=False,sensitivity_mg_count=.244,sensitivity_calibrated=False,window='60/120 nominal MCU seconds, identical samples under two clock scenarios',quality='strict COMPLETE; warmup11 invalid; gaps/nonfinite/rails reset filter; 10s settling invalid; FULL requires every observed sample and full duration',source='XYZ already FIR107 Q15 /10; DC preserved in archive, suppressed by bandpass',normative_comfort_threshold=None)

def hist_quantiles(hist,qs):
 c=np.cumsum(hist,axis=0);n=c[-1]
 # Exact nearest-rank quantiles of archived integer counts, no interpolation.
 return {str(q):[(int(np.searchsorted(c[:,a],max(1,int(np.ceil(q*n[a])))))-32768)*.244 for a in range(3)] for q in qs}

def pooled_rms(n,ss):
 n=np.asarray(n);ss=np.asarray(ss)
 return np.sqrt(np.sum(ss,axis=0)/np.sum(n))
