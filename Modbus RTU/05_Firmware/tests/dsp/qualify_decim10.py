"""Independent SciPy float64/quantized convolution vs compiled C. No hardware.
Run: python qualify_decim10.py <decim10_replay> <fresh-evidence-directory>.
Tolerances fixed before replay: exact quantized Q15/Q31 outputs; f32 <=1 count;
Q15-to-f64 <=ceil(32768*l1(coefficient error)+1) counts for ANY int16 history;
DC exact after startup; gain .1dB (<=25Hz)/.5dB; alias -60dB at A=12000.
"""
import sys,json,hashlib,subprocess,math
from pathlib import Path
import numpy as np
import scipy
from scipy import signal
p=Path(__file__).resolve().parent
exe=Path(sys.argv[1]).resolve();out=Path(sys.argv[2]).resolve();out.mkdir(parents=True,exist_ok=False)
fs=80000/3;fo=fs/10;n=160000
# Regenerate theoretical filter, not reading the C implementation or its state.
h=signal.firwin(107,1333.3333333333333,fs=fs,window=('kaiser',7.85726))
coeff=json.loads((p/'decim10_coefficients.json').read_text());q15=np.array(coeff['q15'])/32768;q31=np.array(coeff['q31'])/2**31;f32=np.array(coeff['float32'])
assert np.max(np.abs(np.array(coeff['float64'])-h))==0
report=dict(numpy=np.__version__,scipy=scipy.__version__,nominal_fs=[80000,3],nominal_output=[8000,3],tolerances=dict(q15_exact=0,q31_exact=0,f32_vs_float64_counts=1,q15_vs_float64_counts=math.ceil(coeff['q15_raw_error_bound_counts']),passband_low_db=.1,passband_high_db=.5,stopband_db=-60,amplitude=12000),witnesses={},runs=[],response={},tones=[],boundaries={})
def oracle(x,c=h):
 y=signal.upfirdn(c,x.astype(np.float64),down=10,axis=0)[:(len(x)+9)//10]
 return np.clip(np.rint(y),-32768,32767).astype('<i2')
def replay(label,x,mode='q15',chunk='1024'):
 src=out/(label+'.in');dst=out/(label+'-'+mode+'-'+chunk+'.out')
 if not src.exists():src.write_bytes(x.astype('<i2').tobytes())
 z=subprocess.run([str(exe),str(src),str(dst),mode,str(chunk)],capture_output=True,text=True,check=True)
 stats=json.loads(z.stdout);raw=dst.read_bytes();y=np.frombuffer(raw,dtype='<i2').reshape(-1,3)
 assert len(y)==(len(x)+9)//10 and stats['output']==len(y) and stats['source']==len(x)
 ref=oracle(x,q15 if mode=='q15' else q31 if mode=='q31' else h)
 err=int(np.max(np.abs(y.astype(np.int32)-ref.astype(np.int32)))) if len(y) else 0
 assert err<= (1 if mode=='f32' else 0),(label,mode,err)
 theoretical=oracle(x);diff=int(np.max(np.abs(y.astype(np.int32)-theoretical.astype(np.int32)))) if len(y) else 0
 assert diff<= (report['tolerances']['q15_vs_float64_counts'] if mode=='q15' else 1),(label,mode,diff)
 row=dict(label=label,mode=mode,chunk=chunk,quality_scope='Q15 production state; alternative modes replace numeric XYZ only',source_sha256=hashlib.sha256(src.read_bytes()).hexdigest(),sha256=hashlib.sha256(raw).hexdigest(),exact_oracle_error_counts=err,float64_error_counts=diff,**stats)
 report['runs'].append(row)
 return y,row
x=np.empty((n,3),dtype=np.int16);x[:]=[16384,-8192,4096]
imp=np.zeros((n,3),dtype=np.int16);imp[0,0]=16384
t=np.arange(n)/fs
mix=np.rint(np.stack([1000+12000*np.sin(2*np.pi*25*t),-2000+11000*np.cos(2*np.pi*700*t),500+7000*np.sin(2*np.pi*1600*t)],axis=1)).astype(np.int16)
expected=['7a90f77cc609e017be51c46ae4d9cac3d8476bdf2abe396742da5c504341a828','27f14a2cb1ff2d01bc9df168a1cabb756f1419b1ad78903a72aa416b28e14995','56696f8d06264bcb24152ec8b705aa209056309b194c1853c9c48dbb5e4e093d']
for (label,x),wanted in zip([('DC',x),('IMP',imp),('MIX',mix)],expected):
 ref=oracle(x);sha=hashlib.sha256(ref.tobytes()).hexdigest();assert sha==wanted,(label,sha,wanted)
 report['witnesses'][label]=dict(sha256=sha,bytes=ref.nbytes,count=len(ref));(out/(label+'-float64.out')).write_bytes(ref.tobytes())
 results={}
 for mode in ['q15','q31','f32']:
  y,row=replay(label,x,mode);results[mode]=row
  if label=='DC' and mode=='q15':assert np.all(y[11:]==[16384,-8192,4096])
 print(label,'witness PASS, max errors', {k:v['float64_error_counts'] for k,v in results.items()},flush=True)
for chunk in ['1','7','13','31','255','512','1024','0']:
 y,row=replay('MIX',mix,chunk=chunk)
 assert row['sha256']==next(r['sha256'] for r in report['runs'] if r['label']=='MIX' and r['mode']=='q15')
# Quantified response over continuum: symmetric centred response Lipschitz bound.
for label,c in [('f64',h),('f32',f32),('q15_exact_dc',q15),('q31_exact_dc',q31)]:
 f=np.linspace(fo-700,fs/2,262145);_,z=signal.freqz(c,worN=f,fs=fs)
 slope=2*np.pi/fs*np.sum(np.abs(np.arange(107)-53)*np.abs(c));bound=np.max(np.abs(z))+slope*(f[1]-f[0])/2+1e-12
 pb=np.linspace(0,700,32769);_,g=signal.freqz(c,worN=pb,fs=fs)
 rounding_bound=(2*(.5*np.abs(c).sum()+.5)/12000) # conservative tone projection of source+output rounding
 row=dict(dc=float(np.sum(c)),passband_error_db=float(np.max(np.abs(20*np.log10(np.abs(g))))),stop_max_db=float(20*np.log10(np.max(np.abs(z)))),grid_bound_db=float(20*np.log10(bound)),tone_bound_with_quantization_db=float(20*np.log10(bound+rounding_bound)),peak_frequency=float(f[np.argmax(np.abs(z))]),group_delay_sources=53,slope_per_hz=float(slope),grid_step_hz=float(f[1]-f[0]))
 assert row['grid_bound_db']<=-60 and row['passband_error_db']<.1
 if label=='q15_exact_dc':assert row['tone_bound_with_quantization_db']<=-60
 report['response'][label]=row
# Actual C int16 time series, fit amplitude and phase after startup (no UTC claim).
frequencies=[2,5,25,100,1000/3,2000/3,700,800,1000,1333,1600,1800,fo-700,2000,fo,3000,4000,6000,10000,13200]
for f in frequencies:
 N=math.ceil(8*fs) if f==2 else math.ceil(2*fs)
 t=np.arange(N)/fs;x=np.rint(np.stack([12000*np.sin(2*np.pi*f*t),12000*np.cos(2*np.pi*f*t),np.zeros(N)],axis=1)).astype(np.int16)
 y,row=replay('tone-'+str(f).replace('.','_'),x)
 signed=((f+fo/2)%fo)-fo/2;alias=abs(signed);sample=np.arange(len(y))*10/fs;take=sample>.1
 amps=[];phases=[]
 for a in range(2):
  if alias<1e-8:amps.append(abs(float(np.mean(y[take,a]))));phases.append(None)
  else:
   design=np.stack([np.sin(2*np.pi*alias*sample[take]),np.cos(2*np.pi*alias*sample[take]),np.ones(np.count_nonzero(take))],axis=1)
   fit=np.linalg.lstsq(design,y[take,a],rcond=None)[0];amps.append(float(np.hypot(fit[0],fit[1])));phases.append(float(math.atan2(fit[1],fit[0])))
 gain=max(amps)/12000;db=20*math.log10(max(gain,1e-15));required=f>=fo-700-1e-8 and alias<=700+1e-8
 if f<=700:assert abs(db)<= (.1 if f<=25 else .5),(f,db)
 if required:assert db<=-60,(f,alias,db)
 phase_error=None
 if f<=700:
  expected_phase=-2*np.pi*f*53/fs;phase_error=math.atan2(math.sin(phases[0]-expected_phase),math.cos(phases[0]-expected_phase));assert abs(phase_error)<.002,(f,phase_error)
 report['tones'].append(dict(hz=f,alias_hz=alias,amplitude_db=db,rejection_required=required,phase_error_rad=phase_error))
 print('tone',f,'alias',alias,'dB',round(db,5),'required',required,flush=True)
# Full-scale, adversarial histories and segmentation-length boundaries.
rng=np.random.default_rng(10710);random=rng.integers(-32768,32768,(160000,3),dtype=np.int16)
edge=np.empty((800,3),dtype=np.int16);edge[:400]=32760;edge[400:]=-32760
adversarial=np.zeros((1070,3),dtype=np.int16)
# An arbitrary bounded input can align quantization error; no tone-only tolerance.
e=np.sign(q15-h);adversarial[1000-np.arange(107),0]=np.where(e>=0,32767,-32768)
for label,x in [('RANDOM',random),('STEP',edge),('ADVERSARIAL',adversarial)]:
 for mode in ['q15','q31','f32']:
  y,row=replay(label,x,mode)
  if label=='STEP' and mode=='q15':assert row['clipped']>0 and row['source_rails']==0
for N in [0,1,2,9,10,11,37,106,107,108,110,111,257]:
 x=np.zeros((N,3),dtype=np.int16);_,row=replay('length'+str(N),x,chunk='7');report['boundaries'][str(N)]=row['output']
# Retain compact measured results, not all generated data (local binaries stay local).
report['pass']=True
(out/'summary.json').write_text(json.dumps(report,indent=2)+'\n');print('D10-00..11 layer checks PASS; runs',len(report['runs']),flush=True)
