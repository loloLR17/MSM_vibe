"""Independent oracles/tolerances fixed before campaign inspection."""
import unittest
import numpy as np
from scipy import signal
from wm import *
class WmTests(unittest.TestCase):
 def test_selected_true_centres_and_continuous_definition(self):
  for p in POINTS:self.assertLess(abs(abs(analogue(p['true_Hz']))-p['rounded_gain']),.0005 if p['rounded_gain']>.1 else .00005)
  # Nominal80 is NOT exact third-octave centre79.4328; they must not be conflated.
  self.assertGreater(abs(abs(analogue(80))-POINTS[-1]['rounded_gain']),.0001)
 def test_complex_response_phase_warp_and_limits(self):
  for fs in [2661.4620215,2679.51622]:
   f=np.array([.5,.8,1,2,4,10,25,80,100]);h=transfer(design(fs),f,fs)
   np.testing.assert_allclose(h,oracle_digital(f,fs),rtol=1e-8,atol=1e-10)
   self.assertLess(max(abs(abs(h)/abs(analogue(f))-1)),.012)
   self.assertEqual(design(fs).shape,(3,6))
   self.assertLess(abs(transfer(design(fs),np.array([0.]),fs)[0]),1e-12)
 def test_known_sinusoids_units_and_axis_max(self):
  fs=500.;t=np.arange(int(160*fs))/fs
  for f in [.5,.8,1,2,4,10,25,80,100]:
   x=np.column_stack([np.sin(2*np.pi*f*t),2*np.sin(2*np.pi*f*t),np.zeros(len(t))])*UNIT
   s=Stream(fs,design(fs));y=s.apply(x,t,np.ones(len(t),bool));r=metrics(y[t>=60]);expected=2*UNIT/np.sqrt(2)*abs(oracle_digital(f,fs))
   self.assertAlmostEqual(r['TR2_WM_A_MAX']/expected,1,delta=.001)
   self.assertEqual(r['winning_axis'],'Y');self.assertAlmostEqual(r['equivalent_v_mm_s'],.028*r['TR2_WM_A_MAX'])
 def test_multitone_and_broadband_causal_convolution(self):
  fs=500.;t=np.arange(int(160*fs))/fs;sos=design(fs)
  x=sum(a*np.sin(2*np.pi*f*t) for f,a in [(1,3),(10,2),(40,4)])
  y=signal.sosfilt(sos,x);expected=np.sqrt(sum((a*abs(oracle_digital(f,fs)))**2/2 for f,a in [(1,3),(10,2),(40,4)]))
  self.assertAlmostEqual(np.sqrt(np.mean(y[t>=60]**2))/expected,1,delta=.001)
  rng=np.random.default_rng(2718)
  for x in [(1+.8*np.sin(2*np.pi*.3*t))*np.sin(2*np.pi*10*t),signal.chirp(t,1,t[-1],40),rng.standard_normal(len(t)),np.where((t>80)&(t<80.03),100.,0)]:
   impulse=signal.sosfilt(sos,np.r_[1.,np.zeros(len(x)-1)]);ind=signal.fftconvolve(x,impulse)[:len(x)];y=signal.sosfilt(sos,x)
   np.testing.assert_allclose(y,ind,rtol=1e-6,atol=1e-8)
 def test_offsets_tilt_quality_chunks_and_invalid_windows(self):
  fs=500.;t=np.arange(100000)/fs;x=np.column_stack([np.ones(len(t))*1019,np.zeros(len(t)),np.ones(len(t))*80]);v=np.ones(len(t),bool)
  a=Stream(fs,design(fs));y=a.apply(x,t,v);self.assertLess(np.nanmax(abs(y)),1e-5);self.assertTrue(np.isnan(y[:5000]).all())
  x[:,0]+=np.where(t>75,10,0)+3*np.sin(2*np.pi*.5*t);a=Stream(fs,design(fs));y=a.apply(x,t,v);self.assertGreater(np.nanmax(abs(y)),1.)
  b=Stream(fs,design(fs));chunks=[b.apply(x[i:i+943],t[i:i+943],v[i:i+943]) for i in range(0,len(x),943)];np.testing.assert_allclose(y,np.concatenate(chunks),equal_nan=True,rtol=1e-8,atol=1e-8)
  v[40000:40100]=False;a=Stream(fs,design(fs));z=a.apply(x,t,v);self.assertTrue(np.isnan(z[40000:45100]).all())
  t[60000:]+=1;a=Stream(fs,design(fs));z=a.apply(x,t,np.ones(len(t),bool));self.assertTrue(np.isnan(z[60000:65000]).all())
  self.assertFalse(old.quality_mask(np.array([[9000,0,0],[np.nan,0,0],[0,0,0]]),np.array([20,20,0])).any())
  for bad in [np.array([[np.nan,0,0]]),np.zeros((0,3))]:
   with self.assertRaises(ValueError):metrics(bad)
if __name__=='__main__':unittest.main()
