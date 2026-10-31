import unittest
import numpy as np
from core import spectrum,candidates,analyse_frame,qualify_minute,temporal_component,compare,THRESHOLDS

class PKTests(unittest.TestCase):
    fs=512.
    def tones(self,f=20,amp=10,T=10,phase=.43):
        t=np.arange(int(T*self.fs))/self.fs;x=amp*np.cos(2*np.pi*f*t+phase)
        return t,np.column_stack([x,x*.5,np.ones(len(x))*1000])
    def find(self,f,band,T=10,amp=10):
        t,x=self.tones(f,amp,T);X,cg=spectrum(x);return candidates(X,cg,len(t),self.fs,0,band)
    def test_analytic_on_off_bins_units_and_boundary(self):
        for f in [1,2,4.9,5,5.1,20,80,99,100]:
            for band,(lo,hi) in [('BV_PK_ACC_1_5',(1,5)),('BV_PK_VEL_5_100',(5,100))]:
                if not lo<=f<=hi:continue
                for off in [0,.037]:
                    actual=f+off
                    if actual>hi:continue
                    with self.subTest(f=actual,band=band):
                        c=self.find(actual,band)[0]
                        target=10*9.80665 if band.endswith('1_5') else 10*9.80665/(2*np.pi*actual)
                        self.assertLess(abs(c['component_pk']/target-1),.01) # fixed1% isolated-tone limit
                        self.assertLess(abs(c['frequency_Hz']-actual),.002)
                        self.assertAlmostEqual(c['component_pk']/c['component_rms'],np.sqrt(2),places=12)
                        if f in [1,5,100] and off==0:self.assertEqual(c['status'],'BAND_EDGE_UNCERTAIN')
        self.assertEqual(self.find(101,'BV_PK_VEL_5_100'),[])
    def test_independent_time_frequency_fit(self):
        t,x=self.tones(20.037,T=10)
        X,cg=spectrum(x);c=candidates(X,cg,len(t),self.fs,0,'BV_PK_VEL_5_100')[0]
        independent=temporal_component(x[:,0],self.fs,c['frequency_Hz'],'BV_PK_VEL_5_100')
        self.assertLess(abs(independent['component_pk']/c['component_pk']-1),.01)
        self.assertLess(abs(independent['frequency_Hz']-20.037),1e-5)
    def test_multitone_not_broadband_and_pruning(self):
        t,x=self.tones(20,T=10);x[:,0]+=10*np.cos(2*np.pi*40*t)
        X,cg=spectrum(x)
        a=candidates(X,cg,len(t),self.fs,0,'BV_PK_VEL_5_100');b=candidates(X,cg,len(t),self.fs,0,'BV_PK_VEL_5_100',prune=False)
        self.assertEqual(a,b)
        self.assertAlmostEqual(a[0]['component_pk'],10*9.80665/(2*np.pi*20),delta=.001)
        broad=np.sqrt(np.mean(x[:,0]**2))*np.sqrt(2)
        self.assertGreater(broad,1.4*a[0]['component_mg_pk'])
    def test_close_lines_AM_FM_impulse_noise(self):
        t,x=self.tones(20,T=60);x[:,0]+=8*np.cos(2*np.pi*20.02*t)
        X,cg=spectrum(x);c=candidates(X,cg,len(t),self.fs,0,'BV_PK_VEL_5_100')[0]
        self.assertNotEqual(c['status'],'SPECTRAL_CANDIDATE')
        t,x=self.tones(20,T=60);x[:,0]*=1+.5*np.cos(2*np.pi*.5*t)
        X,cg=spectrum(x);c=candidates(X,cg,len(t),self.fs,0,'BV_PK_VEL_5_100')[0]
        self.assertEqual(c['status'],'MODULATION_OR_MULTISOURCE')
        x[:,0]=10*np.cos(2*np.pi*20*t+2*np.sin(2*np.pi*.1*t))
        X,cg=spectrum(x);c=candidates(X,cg,len(t),self.fs,0,'BV_PK_VEL_5_100')[0]
        self.assertNotEqual(c['status'],'SPECTRAL_CANDIDATE')
        rng=np.random.default_rng(4);x=rng.normal(size=x.shape);x[len(x)//2,0]+=1000
        X,cg=spectrum(x);c=candidates(X,cg,len(t),self.fs,0,'BV_PK_VEL_5_100')[0]
        self.assertNotEqual(c['status'],'SPECTRAL_CANDIDATE')
    def test_quality_gravity_missing_clip_and_stationarity(self):
        t,x=self.tones(20,T=60);m=analyse_frame(t,x,0,60,self.fs,11)
        c=m[0,'BV_PK_VEL_5_100'];self.assertEqual(c['frame_status'],'COMPLETE')
        short=[]
        for k in range(6):
            take=(t>=k*10)&(t<(k+1)*10)
            short.append(analyse_frame(t[take],x[take],k*10,10,self.fs,11+int(k*10*self.fs))[0,'BV_PK_VEL_5_100'])
        q=qualify_minute(c,short);self.assertTrue(q['comparison_eligible'])
        self.assertEqual(compare(q['reported_pk'],'BV_PK_VEL_5_100',True),'SOUS_REPERE_1PK')
        self.assertEqual(compare(0,'BV_PK_ACC_1_5',False),'INDETERMINATE')
        self.assertEqual(analyse_frame(t,x,0,60,self.fs,0)[0,'BV_PK_ACC_1_5']['frame_status'],'INVALID_FIR_WARMUP')
        x[30,0]=32767*.244;self.assertEqual(analyse_frame(t,x,0,60,self.fs,11)[0,'BV_PK_ACC_1_5']['frame_status'],'INVALID_CLIP_OR_NONFINITE')
        x[30,0]=np.nan;self.assertEqual(analyse_frame(t,x,0,60,self.fs,11)[0,'BV_PK_ACC_1_5']['frame_status'],'INVALID_CLIP_OR_NONFINITE')
        x=np.ones_like(x)*1000;X,cg=spectrum(x);self.assertEqual(candidates(X,cg,len(t),self.fs,0,'BV_PK_ACC_1_5'),[])
        tt=t.copy();tt[len(tt)//2:]+=1;self.assertEqual(analyse_frame(tt,x,0,60,self.fs,11)[0,'BV_PK_ACC_1_5']['frame_status'],'INVALID_TIME_GAP')
        self.assertEqual(analyse_frame(t[:100],x[:100],0,60,self.fs,11)[0,'BV_PK_ACC_1_5']['frame_status'],'PARTIAL_WINDOW')
    def test_reference_values_and_component_sqrt2(self):
        self.assertEqual(THRESHOLDS['BV_PK_ACC_1_5'],[125,141,157]);self.assertEqual(THRESHOLDS['BV_PK_VEL_5_100'],[4,4.5,5])
        # Boundary values are not rounded before classification (exact equality explicit).
        self.assertEqual(compare(4.04,'BV_PK_VEL_5_100',True),'ENTRE_1PK_2PK')
        self.assertEqual(round(4.04,1),4.0)

class ExportTests(unittest.TestCase):
    def test_numpy_scalars_preserve_numeric_and_bool_types(self):
        import tempfile,json
        from pathlib import Path
        from run import dump
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/'out.json';dump(p,dict(agreement=np.bool_(True),value=np.float64(1.25),count=np.int64(3)))
            self.assertEqual(json.loads(p.read_text()),dict(agreement=True,value=1.25,count=3))

if __name__=='__main__':unittest.main(verbosity=2)
