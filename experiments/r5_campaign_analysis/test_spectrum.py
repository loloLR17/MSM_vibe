import tempfile,unittest
from pathlib import Path
import numpy as np
from scipy import signal
from run_analysis import spectral_summary
from analysis import design

class SpectrumTests(unittest.TestCase):
    def test_batched_density_matches_scipy_and_masks_gap(self):
        fs=166.;t=np.arange(16384)/fs
        x=np.column_stack([3*np.sin(2*np.pi*10*t),4*np.sin(2*np.pi*10*t),np.zeros(len(t))])
        with tempfile.TemporaryDirectory() as d:
            info,times,f,p,ridges=spectral_summary(x,fs,Path(d))
            ref_f,ref_p=signal.welch(x,fs=fs,window=signal.windows.hann(2048,sym=False),nperseg=2048,noverlap=1024,detrend=False,axis=0)
            mask=(ref_f>=2)&(ref_f<=25)
            np.testing.assert_allclose(p.mean(axis=0),ref_p[mask],rtol=2e-6,atol=1e-7)
            self.assertLess(abs(f[np.argmax(p.mean(axis=0).sum(axis=1))]-10),.1)
            self.assertAlmostEqual(float(np.sum(p.mean(axis=0))*(f[1]-f[0])),12.5,delta=.001)
            x[:3000]=np.nan
            info,times,f,p,_=spectral_summary(x,fs,Path(d))
            self.assertTrue(np.isnan(p[:3]).all());self.assertGreater(info['invalid_frames'],0)
    def test_out_of_band_alias_attenuation(self):
        fs=2661.462021501383
        # Lowest possible original frequency folding into upper analysed edge.
        gain=abs(signal.sosfreqz(design(fs),worN=[fs/16-25],fs=fs)[1][0])
        self.assertLess(20*np.log10(gain),-60)
        # Finite attenuation, explicitly NOT a proof of perfect alias elimination.
        self.assertGreater(gain,0)

if __name__=='__main__':unittest.main(verbosity=2)
