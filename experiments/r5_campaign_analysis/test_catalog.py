"""Persistence contract fixtures ONLY. Not a .TR2 parser/physical qualification."""
import csv,gzip,json,sqlite3,tempfile,unittest
from pathlib import Path
from catalog import digest,run_import,query_summary

class CatalogTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.root=Path(self.tmp.name);self.a=self.root/'analysis';self.a.mkdir();self.db=self.root/'r5.sqlite'
        source=self.root/'opaque_source_fixture';source.write_bytes(b'persistence fixture, not real .TR2')
        manifest=dict(source=dict(status='COMPLETE',path=str(source),uid=[1,2,3],session=4,format_version=3,firmware_sha='f'*40,observed_duration_us=120000000),whole_file_sha256=digest(source),site='synthetic persistence ONLY',UTC='UTC_UNKNOWN',output_fs_est_Hz=100.,quality='fixture only',algorithm_code_sha256='a'*64)
        (self.a/'manifest.json').write_text(json.dumps(manifest))
        (self.a/'filter.json').write_text(json.dumps(dict(band_Hz=[2,25],method='fixture')))
        for name in ['spectrogram.npz','segments_psd.csv','ridge_candidates.csv']:(self.a/name).write_text('opaque export fixture')
        fields=['start_mcu_s','end_mcu_s','raw_n','band_n','raw_coverage_est','band_coverage_est','status']
        for mode in ['raw','band']:fields += [mode+'_'+k+'_mg' for k in ['rms_x','rms_y','rms_z','rms_global','peak_x','peak_y','peak_z','peak_vector']]
        for width in [1,10,60]:
            p=self.a/('windows_%02ds.csv'%width+('.gz' if width==1 else ''));opener=gzip.open if width==1 else open
            with opener(p,'wt',newline='') as f:
                w=csv.writer(f);w.writerow(fields)
                for start in range(0,120,width):w.writerow([start,start+width,100*width,100*width,1,1,'VALID_ESTIMATED']+[1,2,2,3,4,5,6,7]*2)
        self.refresh()
    def refresh(self):
        files=[dict(name=p.name,sha256=digest(p)) for p in self.a.iterdir() if p.name!='artifacts.json']
        (self.a/'artifacts.json').write_text(json.dumps(files))
    def tearDown(self):self.tmp.cleanup()
    def test_import_queries_idempotent_and_versioned(self):
        a=run_import(self.a,self.db);before=digest(self.db);b=run_import(self.a,self.db)
        self.assertEqual(a['counts'],b['counts']);self.assertEqual(before,digest(self.db));self.assertTrue(b['idempotent'])
        self.assertEqual(a['counts']['window_metrics'],2*(120+12+2))
        q=query_summary(self.db,a['analysis_id']);self.assertEqual(len(q),4);self.assertEqual(q['global_per_minute']['count'],2);self.assertEqual(q['worst_10s']['first_rows'][0]['rms_global'],3)
        m=json.loads((self.a/'manifest.json').read_text());m['algorithm_code_sha256']='b'*64;(self.a/'manifest.json').write_text(json.dumps(m))
        c=run_import(self.a,self.db);self.assertNotEqual(a['analysis_id'],c['analysis_id']);self.assertEqual(c['counts']['campaigns'],1);self.assertEqual(c['counts']['analysis_runs'],2)
    def test_missing_corrupt_and_foreign_db_preserved(self):
        a=run_import(self.a,self.db);before=digest(self.db)
        source=self.root/'opaque_source_fixture';source.write_bytes(b'changed')
        with self.assertRaises(ValueError):run_import(self.a,self.db)
        self.assertEqual(before,digest(self.db));self.assertIn('corrupt',(Path(str(self.db)+'.errors.jsonl')).read_text())
        source.unlink()
        with self.assertRaises(FileNotFoundError):run_import(self.a,self.db)
        source.write_bytes(b'persistence fixture, not real .TR2')
        foreign=self.root/'service.sqlite'
        with sqlite3.connect(foreign) as c:c.execute('CREATE TABLE live_observation(x)')
        before=digest(foreign)
        with self.assertRaises(ValueError):run_import(self.a,foreign)
        self.assertEqual(before,digest(foreign))
    def test_tampered_export_rejected(self):
        (self.a/'segments_psd.csv').write_text('tampered')
        with self.assertRaises(ValueError):run_import(self.a,self.db)
        self.assertFalse(self.db.exists())

if __name__=='__main__':unittest.main(verbosity=2)
