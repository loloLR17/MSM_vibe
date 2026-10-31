"""Non-weighted fallback only. NO Wm coefficients and NO normative equivalence.
R5 strict acceptance/payload decoder/statistics reused; existing files unchanged.
"""
from pathlib import Path
import sys,hashlib,json
import numpy as np
from scipy import signal
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'r5_campaign_analysis'))
from analysis import BandStream, Moments, verified_blocks, reader
VERSION='r5-unweighted-1-80.v1'
SETTLING=10.

def canonical(x):return json.dumps(x,sort_keys=True,separators=(',',':'),allow_nan=False)

def filters(fs):
    """Prewarped analogue Butterworth plus cancelled origin integrator.
    Removing one analogue zero at 0 BEFORE bilinear avoids an unstable digital
    pole-zero cancellation at DC. Velocity is stable trapezoidal integration
    of the same bandpassed acceleration, not equivalent weighted velocity.
    """
    warped=2*fs*np.tan(np.pi*np.array([1.,80.])/fs)
    z,p,k=signal.butter(4,warped,btype='bandpass',analog=True,output='zpk')
    if not np.all(z==0):raise ValueError('unexpected bandpass origin zeros')
    az,ap,ak=signal.bilinear_zpk(z,p,k,fs=fs)
    vz,vp,vk=signal.bilinear_zpk(z[1:],p,k*9.80665,fs=fs)
    return signal.zpk2sos(az,ap,ak),signal.zpk2sos(vz,vp,vk)

class Stream(BandStream):
    def __init__(self,fs,sos):
        # Reuse tested R5 invalid/gap state-machine, different SOS and settle.
        super().__init__(fs)
        self.sos=sos
    def apply(self,x,t,valid):
        # Parent's 5s invalid prefix is extended to 10s by independent age count.
        before=self.age
        out=super().apply(x,t,valid)
        # Recompute age mask consistently across invalid runs and time gaps.
        age=before;previous=self._quality_last if hasattr(self,'_quality_last') else None
        # Validity runs, no per-sample Python iteration over the campaign.
        gaps=np.r_[False,np.diff(t)>10/self.fs]
        if previous is not None:gaps[0]=t[0]-previous>10/self.fs
        boundaries=np.unique(np.r_[0,len(x),np.flatnonzero(valid[1:]!=valid[:-1])+1,np.flatnonzero(gaps)])
        for a,b in zip(boundaries[:-1],boundaries[1:]):
            if gaps[a] or not valid[a]:age=0
            if not valid[a]:continue
            needed=max(0,int(np.ceil(SETTLING*self.fs))-age)
            out[a:a+min(needed,b-a)]=np.nan;age+=b-a
        self._quality_last=float(t[-1])
        return out

def quality_mask(x,indices):
    return (indices>=11)&np.isfinite(x).all(axis=1)&(np.abs(x)<32767*.244).all(axis=1)

def compatible(a,b):
    """Numerical comparability is denied if ANY method/pose assumption differs.
    Campaign SHA is provenance, not a requirement that two campaigns be identical.
    """
    keys=['algorithm','version','edition','filter_sha256','unit','band','window_policy','sensitivity','clock','site','support','orientation','firmware','quality_policy','UTC']
    missing=[k for k in keys if k not in a or k not in b]
    mismatch=[k for k in keys if k in a and k in b and a[k]!=b[k]]
    unknown=[k for k in ['support','orientation','sensitivity','clock'] if isinstance(a.get(k),dict) and a[k].get('qualified') is not True]
    return dict(equivalent_comparison_allowed=not(missing or mismatch or unknown),missing=missing,mismatch=mismatch,unqualified=unknown)

def definition(fs,clock,unit):
    a,v=filters(fs);sos=a if unit=='mg' else v
    return dict(algorithm='causal_Butterworth_unweighted_'+('acceleration' if unit=='mg' else 'physical_velocity_trapezoid'),version=VERSION,edition='NONE_NON_NORMATIVE',filter_sha256=hashlib.sha256(np.asarray(sos,dtype='<f8').tobytes()).hexdigest(),sos=sos.tolist(),unit=unit,band=[1,80],window_policy='nonoverlap60/120 nominal MCU seconds; same archived samples for both clock scenarios; exclude FIR11 and settling10s',sensitivity=dict(mg_per_count=.244,qualified=False),clock=dict(scenario=clock,fs_est_Hz=fs,qualified=False),site='PC machine plancher',support=dict(description='double-face mince annoncé',qualified=False),orientation=dict(description='XYZ capteur; correspondance navire inconnue',qualified=False),firmware='c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e',quality_policy='no gap filling; clipped/nonfinite/warmup invalid; 10s settle after restart; all observed points required for FULL',UTC='UTC_UNKNOWN',qualification='NUMERICALLY_VALIDATED_PHYSICALLY_UNCALIBRATED',normative_equivalence_implemented=False)
