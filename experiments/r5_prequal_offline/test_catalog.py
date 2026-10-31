import tempfile,unittest,sqlite3
from contextlib import closing
from prequal import *
from prequal_catalog import create_copy,import_results,readonly,fingerprints,register_artifacts
from run import csvwrite
class CatalogTests(unittest.TestCase):
 def setUp(self):
  self.temp=tempfile.TemporaryDirectory();self.root=Path(self.temp.name);self.source=self.root/'old.sqlite';self.new=self.root/'new.sqlite';self.out=self.root/'results';self.out.mkdir()
  with closing(sqlite3.connect(self.source)) as c:
   c.executescript("PRAGMA application_id=1380270915;PRAGMA user_version=3;CREATE TABLE campaigns(campaign_id TEXT PRIMARY KEY);INSERT INTO campaigns VALUES('source');CREATE TABLE old_data(k INTEGER PRIMARY KEY,v TEXT);INSERT INTO old_data VALUES(1,'KEEP');")
  d=definition(200.,'TEST_LP10','MCU');mid=hashlib.sha256(canonical(d).encode()).hexdigest();dump(self.out/'definitions.json',{mid:d})
  dump(self.out/'MANIFEST.json',dict(source_whole_sha256='source',core_sha256='a',runner_sha256='b'))
  csvwrite(self.out/'metrics.csv',[dict(method_id=mid,width_s=60,start_mcu_s=0,rms_x=1.,rms_y=2.,rms_z=3.,direction_max=3.,vector_technical=np.sqrt(14))])
 def tearDown(self):self.temp.cleanup()
 def test_safe_copy_history_and_double_import(self):
  before=sha(self.source);create_copy(self.source,self.new);a=import_results(self.out,self.new);snapshot=sha(self.new);b=import_results(self.out,self.new)
  self.assertFalse(a['idempotent']);self.assertTrue(b['idempotent']);self.assertEqual(snapshot,sha(self.new));self.assertEqual(sha(self.source),before)
  with closing(readonly(self.source)) as a,closing(readonly(self.new)) as b:
   self.assertEqual(fingerprints(a),fingerprints(b));self.assertEqual(b.execute('SELECT count(*) FROM prequal_metrics').fetchone()[0],1)
 def test_artifact_audit_idempotence_and_refusal(self):
  create_copy(self.source,self.new);r=import_results(self.out,self.new)
  p=self.out/'definitions.json';first=register_artifacts(self.new,r['analysis_id'],[p]);before=sha(self.new)
  self.assertEqual(first,register_artifacts(self.new,r['analysis_id'],[p]));self.assertEqual(before,sha(self.new))
  p.write_text('changed')
  with self.assertRaises(ValueError):register_artifacts(self.new,r['analysis_id'],[p])
  self.assertEqual(before,sha(self.new))
 def test_inplace_existing_foreign_and_conflict_refused(self):
  before=sha(self.source)
  with self.assertRaises(ValueError):create_copy(self.source,self.source)
  create_copy(self.source,self.new);import_results(self.out,self.new)
  with self.assertRaises(ValueError):create_copy(self.source,self.new)
  with self.assertRaises(ValueError):import_results(self.out,self.source)
  with (self.out/'metrics.csv').open('a') as f:f.write('changed\n')
  with self.assertRaises(ValueError):import_results(self.out,self.new)
  self.assertEqual(before,sha(self.source))
if __name__=='__main__':unittest.main()
