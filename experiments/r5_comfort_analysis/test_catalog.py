"""Aggregate persistence fixtures only, not archive acceptance or Wm qualification."""
import csv,json,sqlite3,tempfile,unittest
from pathlib import Path
from core import canonical,definition
from comfort_catalog import sha,run_import,queries
from run import sha_bytes

class CatalogTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.root=Path(self.tmp.name);self.out=self.root/'analysis';self.out.mkdir();self.db=self.root/'secondary.sqlite'
        p=self.root/'opaque.fixture';p.write_bytes(b'catalogue fixture, NOT .TR2 signal')
        self.manifest=dict(normative_names_enabled=False,normative_thresholds_enabled=False,whole_file_sha256=sha(p),source=dict(path=str(p),status='COMPLETE'),algorithm_code_sha256='a'*64,core_code_sha256='c'*64)
        self.defs={}
        for clock in ['MCU','QPC_ESTIMATE']:
            for unit in ['mg','mm/s']:
                d=definition(4096.,clock,unit);mid=sha_bytes(canonical(d).encode());self.defs[mid]=d
        self.write()
    def write(self):
        (self.out/'manifest.json').write_text(json.dumps(self.manifest));(self.out/'definitions.json').write_text(json.dumps(self.defs))
        fields=['metric_definition_id','unit','clock','window_mcu_s','start_mcu_s','end_mcu_s','observed_n','valid_n','coverage_observed','quality','rms_x','rms_y','rms_z','direction_max','vector_technical','peak_vector']
        with (self.out/'metrics.csv').open('w',newline='') as f:
            w=csv.writer(f,lineterminator='\n');w.writerow(fields)
            for mid,d in self.defs.items():
                for width in [60,120]:w.writerow([mid,d['unit'],d['clock']['scenario'],width,0,width,100,100,1,'FULL',3,4,0,4,5,7])
        for name in ['summary.json','clock_sensitivity.csv','fir_response.csv','real_crosschecks.json']:(self.out/name).write_text('[]')
        (self.out/'artifacts.json').write_text(json.dumps([dict(name=p.name,sha256=sha(p)) for p in self.out.iterdir() if p.name!='artifacts.json']))
    def tearDown(self):self.tmp.cleanup()
    def test_import_queries_idempotence_and_new_cohort_preserved(self):
        a=run_import(self.out,self.db);before=sha(self.db);b=run_import(self.out,self.db)
        self.assertTrue(b['idempotent']);self.assertEqual(a['counts'],b['counts']);self.assertEqual(before,sha(self.db))
        self.assertEqual(a['counts']['window_metrics'],8);self.assertEqual(a['counts']['metric_definitions'],4)
        q=queries(self.db);self.assertEqual(q['catalogue']['count'],8);self.assertEqual(q['worst_60s_velocity']['first_rows'][0]['direction_max'],4)
        self.manifest['algorithm_code_sha256']='b'*64;self.write();c=run_import(self.out,self.db)
        self.assertEqual(c['counts']['analysis_runs'],8);self.assertEqual(c['counts']['window_metrics'],16);self.assertEqual(c['counts']['campaigns'],1)
    def test_old_schema1_foreign_source_corrupt_and_errors(self):
        with sqlite3.connect(self.db) as c:c.execute('PRAGMA application_id=1380270913');c.execute('PRAGMA user_version=1');c.execute('CREATE TABLE untouched(x)')
        before=sha(self.db)
        with self.assertRaises(ValueError):run_import(self.out,self.db)
        self.assertEqual(before,sha(self.db))
        Path(self.manifest['source']['path']).write_bytes(b'corrupt')
        with self.assertRaises(ValueError):run_import(self.out,self.root/'new.sqlite')
        self.assertFalse((self.root/'new.sqlite').exists());self.assertTrue((self.root/'new.sqlite.errors.jsonl').exists())
    def test_P0_fail_closed(self):
        self.manifest['normative_names_enabled']=True;self.write()
        with self.assertRaises(ValueError):run_import(self.out,self.db)
        self.assertFalse(self.db.exists())

if __name__=='__main__':unittest.main(verbosity=2)
