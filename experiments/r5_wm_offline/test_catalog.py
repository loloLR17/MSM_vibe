import unittest,tempfile,sqlite3,csv
from contextlib import closing
from wm import *
from wm_catalog import create_copy,import_results,readonly,all_old,register
class CatalogTests(unittest.TestCase):
 def setUp(self):
  self.temp=tempfile.TemporaryDirectory();self.root=Path(self.temp.name);self.old=self.root/'v4.sqlite';self.new=self.root/'v5.sqlite';self.out=self.root/'results';self.out.mkdir()
  with closing(sqlite3.connect(self.old)) as c:c.executescript("PRAGMA application_id=1380270915;PRAGMA user_version=4;CREATE TABLE campaigns(campaign_id TEXT PRIMARY KEY);INSERT INTO campaigns VALUES('src');CREATE TABLE prequal_metrics(v INTEGER);INSERT INTO prequal_metrics VALUES(123);CREATE TABLE pk_components(v TEXT);INSERT INTO pk_components VALUES('preserve');")
  d=definition(500,'MCU');mid=__import__('hashlib').sha256(canonical(d).encode()).hexdigest();dump(self.out/'definitions.json',{mid:d});dump(self.out/'MANIFEST.json',dict(source_whole_sha256='src',core_sha256='c',runner_sha256='r'))
  with (self.out/'metrics.csv').open('w',newline='') as f:
   w=csv.DictWriter(f,fieldnames=['method_id','unit','width_s','start_mcu_s','rms_x','rms_y','rms_z','direction_max','TR2_WM_A_MAX','equivalent_velocity_mm_s']);w.writeheader();w.writerow(dict(method_id=mid,unit='mm/s2',width_s=60,start_mcu_s=0,rms_x=1,rms_y=2,rms_z=3,direction_max=3,TR2_WM_A_MAX=3,equivalent_velocity_mm_s=.084))
 def tearDown(self):self.temp.cleanup()
 def test_full_cumulative_preservation_and_idempotence(self):
  before=sha(self.old);create_copy(self.old,self.new);a=import_results(self.out,self.new);snap=sha(self.new);b=import_results(self.out,self.new);self.assertEqual(snap,sha(self.new));self.assertFalse(a['idempotent']);self.assertTrue(b['idempotent']);self.assertEqual(before,sha(self.old))
  with closing(readonly(self.old)) as x,closing(readonly(self.new)) as y:self.assertEqual(all_old(x),all_old(y))
  a=register(self.new,a['analysis_id'],[self.out/'definitions.json']);snap=sha(self.new);b=register(self.new,b['analysis_id'],[self.out/'definitions.json']);self.assertEqual(a,b);self.assertEqual(snap,sha(self.new))
 def test_no_overwrite_inplace_old_catalogue_or_conflicts(self):
  before=sha(self.old)
  with self.assertRaises(ValueError):create_copy(self.old,self.old)
  create_copy(self.old,self.new);a=import_results(self.out,self.new)
  with self.assertRaises(ValueError):create_copy(self.old,self.new)
  with self.assertRaises(ValueError):import_results(self.out,self.old)
  with (self.out/'metrics.csv').open('a') as f:f.write('conflicting\n')
  snap=sha(self.new)
  with self.assertRaises(ValueError):import_results(self.out,self.new)
  self.assertEqual(snap,sha(self.new));self.assertEqual(before,sha(self.old))
if __name__=='__main__':unittest.main()
