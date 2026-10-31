"""Oracles/tolerances defined before full campaign execution."""
import unittest
import numpy as np
from scipy import signal
from prequal import *
from run import comparison

class NumericalTests(unittest.TestCase):
 def test_frequency_response_against_closed_form(self):
  for fs in [2661.462,2679.516]:
   for name in BANDS:
    f=np.array([.3,.5,.8,1,1.2,2,5,10,20,40,80,100])
    np.testing.assert_allclose(abs(transfer(design(fs,name),f,fs)),oracle_gain(f,fs,name),rtol=1e-8,atol=1e-11)
 def test_sines_multitone_am_fm_parseval_independent(self):
  fs=500.;t=np.arange(int(160*fs))/fs
  # Rational cycle counts, analytic steady-state power; no fitted expected value.
  for name in ['FLAT_1_80','TEST_LP10','BAND_1_2']:
   sos=design(fs,name)
   for f in [.3,.5,.8,1,1.2,2,5,10,20,40,80,100]:
    x=3*np.sin(2*np.pi*f*t)+1019
    y=signal.sosfilt(sos,x,zi=signal.sosfilt_zi(sos)*x[0])[0]
    actual=np.sqrt(np.mean(y[t>=60]**2));expected=3/np.sqrt(2)*oracle_gain(f,fs,name)
    self.assertAlmostEqual(actual/expected,1,delta=.005)
   x=sum(a*np.sin(2*np.pi*f*t) for f,a in [(1,3),(10,2),(40,4)])
   y=signal.sosfilt(sos,x);expected=np.sqrt(sum((a*oracle_gain(f,fs,name))**2/2 for f,a in [(1,3),(10,2),(40,4)]))
   self.assertAlmostEqual(np.sqrt(np.mean(y[t>=60]**2))/expected,1,delta=.001)
  # Whole broadband Parseval, including modulation and impulses, against FFT filtering.
  for x in [(1+.7*np.cos(2*np.pi*.3*t))*np.sin(2*np.pi*10*t),signal.chirp(t,2,t[-1],60),np.where((t>75)&(t<75.05),40.,0.)]:
   h=signal.sosfilt(design(fs,'TEST_LP10'),np.r_[1.,np.zeros(len(x)-1)])
   # Independent finite impulse response convolution of same causal LTI system.
   z=signal.fftconvolve(x,h)[:len(x)];y=signal.sosfilt(design(fs,'TEST_LP10'),x)
   np.testing.assert_allclose(z,y,rtol=1e-6,atol=1e-8)
   X=np.fft.rfft(y);w=np.full(len(X),2.);w[0]=w[-1]=1
   self.assertAlmostEqual(np.sum(w*abs(X)**2)/len(y),np.sum(y*y),delta=1e-7)
 def test_offset_gravity_and_quality(self):
  fs=500.;t=np.arange(100000)/fs;x=np.tile([1019.,-35.,20.],(len(t),1));valid=np.ones(len(t),bool)
  s=Stream(fs,design(fs,'FLAT_1_80'));y=s.apply(x,t,valid)
  self.assertTrue(np.isnan(y[:5000]).all());self.assertLess(np.nanmax(abs(y)),1e-6)
  x[:,0]+=10*(t>60)+5*np.sin(2*np.pi*.3*t) # offset/tilt are real signal, not magically vibration-only.
  s=Stream(fs,design(fs,'FLAT_1_80'));y=s.apply(x,t,valid);self.assertGreater(np.nanmax(abs(y[:,0])),1)
  valid[40000:40050]=False;s=Stream(fs,design(fs,'FLAT_1_80'));y=s.apply(x,t,valid)
  self.assertTrue(np.isnan(y[40000:45050]).all())
  t[60000:]+=1;s=Stream(fs,design(fs,'FLAT_1_80'));y=s.apply(x,t,np.ones(len(t),bool));self.assertTrue(np.isnan(y[60000:65000]).all())
  self.assertFalse(old.quality_mask(np.array([[9000.,0,0],[np.nan,0,0],[0,0,0]]),np.array([20,20,0])).any())
 def test_stream_chunk_invariance_and_energy_pooling(self):
  fs=200.;t=np.arange(60000)/fs;x=np.column_stack([np.sin(2*np.pi*t)*(1+.5*np.sin(2*np.pi*.1*t)),np.cos(2*np.pi*20*t),np.ones(len(t))*1000]);v=np.ones(len(t),bool)
  a=Stream(fs,design(fs,'TEST_LP10'));all_y=a.apply(x,t,v)
  b=Stream(fs,design(fs,'TEST_LP10'));pieces=[b.apply(x[i:i+777],t[i:i+777],v[i:i+777]) for i in range(0,len(t),777)];np.testing.assert_allclose(all_y,np.concatenate(pieces),equal_nan=True,rtol=1e-10,atol=1e-10)
  # Statistically sufficient energies combine, arithmetic mean RMS does not.
  self.assertAlmostEqual(float(pooled_rms([2,2],[[2],[18]])[0]),np.sqrt(5))
  self.assertNotAlmostEqual(np.sqrt(5),2.)
 def test_histogram_quantiles_exact_and_null(self):
  h=np.zeros((65536,3),int);h[32768+10]=2;h[32768-20]=1
  self.assertEqual(hist_quantiles(h,[0,.5,1]),{'0':[-4.88]*3,'0.5':[2.44]*3,'1':[2.44]*3})
  self.assertIsNone(clean(np.nan));self.assertEqual(clean(np.bool_(True)),True)
if __name__=='__main__':unittest.main()
