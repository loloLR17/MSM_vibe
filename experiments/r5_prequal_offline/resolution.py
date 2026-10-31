"""Known analogue synthetic inputs only, never requantize the real archive."""
import argparse,re,struct
from prequal import *

def quantize(x,step):return np.clip(np.rint(x/step),-32768,32767)*step

def simulate():
 fs=26667.;t=np.arange(int(40*fs))/fs;fout=fs/10
 p=ROOT/'Modbus RTU/05_Firmware/src/domain/acquisition/fir_decimator10_coefficients.h'
 q=np.array(list(map(int,re.findall(r'-?\d+',p.read_text().split('= {',1)[1].split('}',1)[0]))),dtype=np.int16)
 assert len(q)==107 and hashlib.sha256(q.astype('<i2').tobytes()).hexdigest()=='0e528c6d985207c265ccaa4a5df797ccdfa5ccfe938ad9a51a6a6ad8c94cc1b4'
 h=q.astype(float)/32768;rows=[]
 for amplitude in [.3,3.,30.]:
  for density in [0.,.075]:
   rng=np.random.default_rng(20261031)
   # HYPOTHESIS only: white Gaussian density extended below characterization band.
   analogue=1019+amplitude*np.sin(2*np.pi*1.3*t)+.7*amplitude*np.sin(2*np.pi*20*t)+density*np.sqrt(fs/2)*rng.standard_normal(len(t))
   reference=signal.lfilter(h,[1],analogue)[::10];tt=np.arange(len(reference))/fout;take=tt>=20
   def metrics(x):
    y=signal.sosfilt(design(fout,'FLAT_1_80'),x,zi=signal.sosfilt_zi(design(fout,'FLAT_1_80'))*x[0])[0]
    z=signal.sosfilt(design(fout,'TEST_LP10'),x,zi=signal.sosfilt_zi(design(fout,'TEST_LP10'))*x[0])[0]
    f=np.fft.rfftfreq(take.sum(),1/fout);X=np.fft.rfft(y[take]);gain=np.zeros(len(f),complex);gain[f>0]=9.80665/(2j*np.pi*f[f>0]);velocity=np.fft.irfft(X*gain,n=take.sum())
    return dict(flat_rms_mg=np.sqrt(np.mean(y[take]**2)),test_rms_mg=np.sqrt(np.mean(z[take]**2)),integrated_velocity_mm_s=np.sqrt(np.mean(velocity**2)),line13_peak_mg=2*abs(X[np.argmin(abs(f-1.3))])/take.sum())
   ref=metrics(reference)
   for g,step in [(8,.244),(4,.122),(2,.061)]:
    # Input quantization followed by archived-Q15 FIR coefficient response and
    # independent round-to-count at output. Integer accumulator isn't emulated.
    raw=quantize(analogue,step);filtered=signal.lfilter(h,[1],raw)[::10];archived=quantize(filtered,step);v=metrics(archived)
    rows.append(dict(label='SYNTHETIC_HYPOTHESIS_NOT_MEASURED',fs_g=g,step_mg=step,signal_13_peak_mg=amplitude,white_noise_density_mg_sqrtHz_assumed=density,raw_clips=int(np.count_nonzero(abs(analogue/step)>32767)),reference=ref,quantized=v,relative_bias={k:v[k]/ref[k]-1 for k in v},RMS_output_difference_mg=np.sqrt(np.mean((archived[take]-reference[take])**2))))
 budget=[dict(label='NOMINAL_COMPUTATION',fs_g=g,step_mg=step,half_LSB_mg=step/2,uniform_quantization_RMS_mg=step/np.sqrt(12),input_quantization_density_ug_sqrtHz=1000*step/np.sqrt(6*fs),output_rounding_density_ug_sqrtHz=1000*step/np.sqrt(6*fout),conditional_white_XY_1_80_RMS_mg=.075*np.sqrt(79),conditional_white_Z_1_80_RMS_mg=.110*np.sqrt(79),white_noise_scope='hypothesis extension to1Hz; ST only100Hz–6.3kHz, independent of FS') for g,step in [(8,.244),(4,.122),(2,.061)]]
 return dict(model='Known synthetic analogue; nominal integer quantizers and linear Q15 FIR; NOT recovery of lost real bits; white model optional nonphysical below100Hz; no integer DSP requalification',sample_rate_native_nominal=fs,budget=budget,simulations=rows)
if __name__=='__main__':
 ap=argparse.ArgumentParser();ap.add_argument('output',type=Path);a=ap.parse_args()
 if a.output.exists():raise ValueError('refuse overwriting simulation evidence')
 dump(a.output,simulate())
