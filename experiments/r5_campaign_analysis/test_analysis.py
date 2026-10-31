"""Synthetic gates: executable acceptance limits, independent formulae, no hardware."""
import unittest
import numpy as np
from scipy import signal, integrate
from analysis import BandStream, Moments, design, velocity_spectral

class AnalysisTests(unittest.TestCase):
    fs = 1024.

    def filtered(self, x):
        t = np.arange(len(x))/self.fs
        return BandStream(self.fs).apply(x, t, np.ones(len(x), bool))

    def test_dc_gravity(self):
        x = np.tile([0., 0., 1000.], (int(12*self.fs), 1))
        y = self.filtered(x)
        self.assertLess(np.nanmax(np.abs(y)), 1e-8)
        self.assertEqual(np.sqrt(np.mean(x[:,2]**2)), 1000)
        self.assertTrue(np.isnan(y[:int(5*self.fs)]).all())

    def test_sinusoids_edges_and_stopband(self):
        t = np.arange(int(20*self.fs))/self.fs
        sos = design(self.fs)
        for f in [2,10,25,80]:
            with self.subTest(f=f):
                x = np.repeat((100*np.sin(2*np.pi*f*t))[:,None],3,axis=1)
                y = self.filtered(x)[int(10*self.fs):,0]
                gain = abs(signal.sosfreqz(sos, worN=[f], fs=self.fs)[1][0])
                self.assertAlmostEqual(np.sqrt(np.mean(y*y)), 100/np.sqrt(2)*gain, delta=.002)
                if f in [2,25]: self.assertAlmostEqual(gain, 1/np.sqrt(2), places=10)
                if f==80: self.assertLess(gain,.01)

    def test_chunk_invariance_and_impulse(self):
        rng = np.random.default_rng(4)
        x = rng.normal(size=(int(16*self.fs),3))
        x[int(7*self.fs),:] += 1000
        t = np.arange(len(x))/self.fs
        whole = self.filtered(x)
        stream = BandStream(self.fs)
        split = np.concatenate([stream.apply(x[a:a+333],t[a:a+333],np.ones(len(x[a:a+333]),bool)) for a in range(0,len(x),333)])
        np.testing.assert_allclose(whole,split,rtol=1e-12,atol=1e-12,equal_nan=True)
        self.assertTrue(np.isfinite(whole[int(5*self.fs):]).all())
        self.assertLess(np.max(np.abs(whole[-100:])),10)

    def test_multitone_energy(self):
        t=np.arange(int(30*self.fs))/self.fs
        x=sum(a*np.sin(2*np.pi*f*t) for f,a in [(2,3),(10,7),(25,5),(80,9)])
        y=self.filtered(np.repeat(x[:,None],3,axis=1))[int(10*self.fs):,0]
        freqs=np.array([2,10,25,80]);amp=np.array([3,7,5,9])
        h=signal.sosfreqz(design(self.fs),worN=freqs,fs=self.fs)[1]
        self.assertAlmostEqual(np.mean(y*y),np.sum((amp*abs(h))**2)/2,delta=.001)

    def test_gap_invalid_and_empty(self):
        t=np.arange(int(20*self.fs))/self.fs
        x=np.ones((len(t),3));valid=np.ones(len(t),bool);valid[:11]=False;valid[8192:8292]=False
        y=BandStream(self.fs).apply(x,t,valid)
        self.assertTrue(np.isnan(y[:11+int(5*self.fs)]).all())
        self.assertTrue(np.isnan(y[8192:8292+int(5*self.fs)]).all())
        m=Moments(30);m.add(t,y);g=m.groups(1)
        self.assertTrue(np.isnan(g['global_rms'][25]))
        t[10240:]+=2
        stream=BandStream(self.fs);y=stream.apply(x,t,np.ones(len(t),bool))
        self.assertEqual(stream.gaps,1)
        self.assertTrue(np.isnan(y[10240:10240+int(5*self.fs)]).all())

    def test_windows_weighting_vector_peak_boundaries(self):
        m=Moments(120)
        t=np.array([0.,.999,1.,9.999,10.,59.999,60.])
        x=np.array([[3,0,0],[0,4,0],[0,0,0],[0,0,0],[0,0,0],[0,0,0],[0,0,12]],float)
        m.add(t[:3],x[:3]);m.add(t[3:],x[3:])
        g=m.groups(10)
        self.assertEqual(g['n'][0],4)
        self.assertEqual(g['vector_peak'][0],4) # NOT sqrt(3²+4²)
        self.assertEqual(g['global_rms'][0],2.5)
        self.assertTrue(np.isnan(g['global_rms'][2]))
        h=m.groups(60)
        self.assertAlmostEqual(h['global_rms'][0],np.sqrt(25/6))
        self.assertEqual(h['vector_peak'][1],12)

    def test_psd_parseval_and_peak(self):
        fs=1024;n=8192;t=np.arange(n)/fs
        x=3*np.sin(2*np.pi*10*t)+5*np.sin(2*np.pi*25*t)
        f,p=signal.welch(x,fs=fs,window=signal.windows.hann(n,sym=False),nperseg=n,noverlap=0,detrend=False)
        self.assertAlmostEqual(np.sum(p)*(fs/n),17,places=10)
        self.assertEqual(f[np.argmax(p)],25)

    def test_velocity_seven_frequencies_offset_multitone(self):
        fs=4096.;t=np.arange(int(8*fs))/fs
        for f in [1,2,5,10,20,40,80]:
            x=1000+10*np.sin(2*np.pi*f*t)
            v=velocity_spectral(x,fs)
            expected=10*9.80665/(2*np.pi*f*np.sqrt(2))
            self.assertAlmostEqual(np.sqrt(np.mean(v*v)),expected,delta=expected*1e-10)
            independent=integrate.cumulative_trapezoid(x-x.mean(),dx=1/fs,initial=0)*9.80665
            independent-=independent.mean()
            self.assertLess(abs(np.sqrt(np.mean(independent**2))/expected-1),.002)
        x=1000+sum(a*np.sin(2*np.pi*f*t) for f,a in [(1,2),(10,3),(80,4)])
        expected=np.sqrt(sum((a*9.80665/(2*np.pi*f*np.sqrt(2)))**2 for f,a in [(1,2),(10,3),(80,4)]))
        self.assertAlmostEqual(np.sqrt(np.mean(velocity_spectral(x,fs)**2)),expected,places=10)
        v=velocity_spectral(1000+np.sin(2*np.pi*.5*t)+np.sin(2*np.pi*100*t),fs)
        self.assertLess(np.max(abs(v)),1e-10)

if __name__=='__main__':unittest.main(verbosity=2)
