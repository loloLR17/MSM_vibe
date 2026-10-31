import copy,unittest
import numpy as np
from scipy import signal,integrate
from core import filters,Stream,quality_mask,definition,compatible

def analytic_response(f,fs):
    # Independent continuous Butterworth prototype, then exact bilinear mapping.
    o1,o2=2*fs*np.tan(np.pi*np.array([1,80])/fs)
    omega=2*fs*np.tan(np.pi*np.asarray(f)/fs)
    s=1j*omega;u=(s*s+o1*o2)/((o2-o1)*s)
    poles=np.exp(1j*np.pi*(2*np.arange(4)+5)/8)
    a=np.prod(-poles)/np.prod(u[...,None]-poles,axis=-1)
    return a,a*9.80665/s

class PhysicalTests(unittest.TestCase):
    fs=4096.
    def test_gain_phase_against_analytic_prototype(self):
        f=np.array([.05,.25,1,2,5,10,20,40,80,160,500])
        for sos,expected in zip(filters(self.fs),analytic_response(f,self.fs)):
            actual=signal.sosfreqz(sos,worN=f,fs=self.fs)[1]
            np.testing.assert_allclose(actual,expected,rtol=1e-8,atol=1e-9)
        a,_=analytic_response(np.array([1,80]),self.fs)
        np.testing.assert_allclose(abs(a),1/np.sqrt(2),rtol=1e-12)
    def test_seven_sines_time_rms_and_physical_integration(self):
        t=np.arange(int(40*self.fs))/self.fs
        for f in [1,2,5,10,20,40,80]:
            x=np.repeat((10*np.sin(2*np.pi*f*t))[:,None],3,axis=1);valid=np.ones(len(t),bool)
            for sos,h in zip(filters(self.fs),analytic_response(float(f),self.fs)):
                y=Stream(self.fs,sos).apply(x,t,valid)[int(20*self.fs):,0]
                self.assertAlmostEqual(np.sqrt(np.mean(y*y)),10*abs(h)/np.sqrt(2),delta=.0001)
            # Separate physical periodic integration oracle, NO normative conversion.
            v=integrate.cumulative_trapezoid(x[:,0],dx=1/self.fs,initial=0)*9.80665;v-=v.mean()
            ideal=10*9.80665/(2*np.pi*f*np.sqrt(2))
            self.assertLess(abs(np.sqrt(np.mean(v*v))/ideal-1),.002)
    def test_dc_tilt_impulse_noise_and_multitone(self):
        t=np.arange(int(40*self.fs))/self.fs
        dc=np.tile([0.,0.,1000.],(len(t),1));valid=np.ones(len(t),bool)
        for sos in filters(self.fs):
            y=Stream(self.fs,sos).apply(dc,t,valid)
            self.assertLess(np.nanmax(abs(y)),1e-7)
        # slow changing tilt projects 1g into X: NOT assumed separable from vibration
        x=dc.copy();x[:,0]=10*np.sin(2*np.pi*.1*t)
        a,_=analytic_response(.1,self.fs)
        y=Stream(self.fs,filters(self.fs)[0]).apply(x,t,valid)[int(20*self.fs):,0]
        self.assertAlmostEqual(np.sqrt(np.mean(y*y)),10*abs(a)/np.sqrt(2),delta=.0001)
        rng=np.random.default_rng(12);x=rng.normal(scale=.1,size=dc.shape)+dc;x[int(20*self.fs)]+=1000
        for sos in filters(self.fs):
            y=Stream(self.fs,sos).apply(x,t,valid)
            self.assertTrue(np.isfinite(y[int(10*self.fs):]).all());self.assertLess(np.nanmax(abs(y)),1000)
        tones=[(2,3),(10,5),(40,7)]
        x=np.repeat(sum(amp*np.sin(2*np.pi*f*t) for f,amp in tones)[:,None],3,axis=1)
        for k,sos in enumerate(filters(self.fs)):
            y=Stream(self.fs,sos).apply(x,t,valid)[int(20*self.fs):,0]
            oracle=sum(amp**2*abs(analytic_response(f,self.fs)[k])**2/2 for f,amp in tones)
            self.assertAlmostEqual(np.mean(y*y),oracle,delta=.0001)
    def test_clipping_gaps_missing_chunk_boundary(self):
        fs=self.fs;t=np.arange(int(24*fs))/fs;x=np.tile([1.,2.,1000.],(len(t),1));x[100]=32767*.244
        valid=quality_mask(x,np.arange(len(t)));self.assertFalse(valid[:11].any());self.assertFalse(valid[100])
        valid[55000:55010]=False
        t[60000:]+=1
        sos=filters(fs)[1];whole=Stream(fs,sos).apply(x,t,valid)
        stream=Stream(fs,sos);parts=[]
        for a in range(0,len(x),777):parts.append(stream.apply(x[a:a+777],t[a:a+777],valid[a:a+777]))
        np.testing.assert_allclose(whole,np.concatenate(parts),rtol=1e-10,atol=1e-10,equal_nan=True)
        self.assertTrue(np.isnan(whole[60000:60000+int(10*fs)]).all())
        self.assertEqual(stream.gaps,1)
    def test_direction_max_is_not_vector(self):
        r=np.array([3.,4.,0.]);self.assertEqual(max(r),4);self.assertEqual(np.linalg.norm(r),5)
    def test_comparability_refuses_unqualified_or_changed_method(self):
        a=definition(2661.462,'MCU','mg');b=copy.deepcopy(a)
        self.assertFalse(compatible(a,b)['equivalent_comparison_allowed'])
        for d in [a,b]:
            for k in ['clock','support','orientation','sensitivity']:d[k]['qualified']=True
        self.assertTrue(compatible(a,b)['equivalent_comparison_allowed'])
        b['filter_sha256']='different';self.assertFalse(compatible(a,b)['equivalent_comparison_allowed'])
        del b['clock'];self.assertIn('clock',compatible(a,b)['missing'])


class WindowTests(unittest.TestCase):
    def test_partial_invalid_and_duration(self):
        from run import export_windows
        from core import Moments
        fs=2.;t=np.arange(240)/fs;x=np.ones((240,3));m=Moments(120)
        m.add(t[11:],x[11:]);observed=np.full(120,2)
        rows=export_windows(m,observed,119.5,fs,60)
        self.assertEqual([r['quality'] for r in rows],['PARTIAL_OR_INVALID','FULL'])
        self.assertEqual(rows[0]['valid_n'],109);self.assertEqual(rows[1]['valid_n'],120)
        self.assertEqual(export_windows(m,observed,119.5,fs,120)[0]['quality'],'PARTIAL_OR_INVALID')
        empty=Moments(120);rows=export_windows(empty,observed,119.5,fs,60)
        self.assertTrue(np.isnan(rows[0]['direction_max']));self.assertEqual(rows[0]['valid_n'],0)
    def test_no_normative_definition_without_P0(self):
        d=definition(2661.462,'MCU','mm/s')
        self.assertEqual(d['edition'],'NONE_NON_NORMATIVE');self.assertFalse(d['normative_equivalence_implemented'])

if __name__=='__main__':unittest.main(verbosity=2)
