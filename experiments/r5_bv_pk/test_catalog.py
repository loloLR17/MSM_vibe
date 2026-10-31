"""SQLite copy/migration and PK aggregate fixture tests. Not a .TR2 proof."""
import csv,gzip,json,sqlite3,tempfile,unittest
from pathlib import Path
from pk_catalog import create_migrated_copy,import_results,legacy_snapshot,query_summary,sha,signature
from run import FIELDS,ROOT
from core import BANDS

class CatalogTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.root=Path(self.tmp.name);self.out=self.root/'results';self.out.mkdir();self.old=self.root/'original-v2.sqlite';self.new=self.root/'unified-v3.sqlite'
        source=self.root/'opaque.fixture';source.write_bytes(b'aggregate database fixture, NOT sensor .TR2')
        self.campaign=sha(source)
        with sqlite3.connect(self.old) as c:
            c.executescript((ROOT/'experiments/r5_comfort_analysis/schema_v2.sql').read_text())
            c.execute('INSERT INTO campaigns VALUES(?,?,?)',(self.campaign,str(source),'{}'))
            c.execute('INSERT INTO metric_definitions VALUES(?,?,?,?,?,?,?,?)',('old_method','v1','NONE_NON_NORMATIVE','mg',1,80,'UNKNOWN','{}'))
            c.execute('INSERT INTO analysis_runs VALUES(?,?,?,?,?,?)',('old_run',self.campaign,'old_method','c'*64,'r'*64,'FIXTURE'))
            c.execute('INSERT INTO window_metrics VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)',('old_run',60,0,60,100,100,1,'FULL',3,4,0,4,5,7))
        self.m=dict(source=dict(path=str(source),status='COMPLETE'),whole_file_sha256=self.campaign,runner_sha256='c'*64)
        self.methods={}
        for band in BANDS:
            for clock in ['MCU','QPC_ESTIMATE']:
                d=dict(edition='BV_NR467_F_2026_07_PK',derived_method_version='fixture1',CF=1,band=band,unit=BANDS[band][2],clock=clock)
                self.methods[signature(d)]=d
        self.write()
    def write(self):
        (self.out/'manifest.json').write_text(json.dumps(self.m));(self.out/'methods.json').write_text(json.dumps(self.methods))
        with gzip.open(self.out/'components.csv.gz','wt',newline='') as f:
            w=csv.DictWriter(f,fieldnames=FIELDS);w.writeheader()
            for mid,d in self.methods.items():
                for axis in 'XYZ':
                    r={k:None for k in FIELDS};r.update(method_id=mid,clock=d['clock'],band=d['band'],unit=d['unit'],axis=axis,start_mcu_s=60,end_mcu_s=119.999,window_mcu_s=60,sample_n=100,valid_input_n=100,frame_status='COMPLETE',status='QUASI_STATIONARY_COMPONENT',frequency_Hz=2 if d['band'].endswith('1_5') else 20,component_pk=2,component_rms=2/2**.5,reported_pk=2,contrast_dB=30,shape_error=.01,comparison_eligible=True,comparison_zone='SOUS_REPERE_1PK');w.writerow(r)
        for name in ['summary.json','independent_real.json','fir_response.json']:(self.out/name).write_text('[]')
        (self.out/'artifacts.json').write_text(json.dumps([dict(name=p.name,sha256=sha(p)) for p in self.out.iterdir() if p.name!='artifacts.json']))
    def tearDown(self):self.tmp.cleanup()
    def test_safe_copy_full_preservation_idempotent_queries_and_new_analysis(self):
        old_sha=sha(self.old);audit=create_migrated_copy(self.old,self.new);a=import_results(self.out,self.new);before=sha(self.new);b=import_results(self.out,self.new)
        self.assertTrue(b['idempotent']);self.assertEqual(before,sha(self.new));self.assertEqual(old_sha,sha(self.old));self.assertEqual(a['counts']['pk_components'],12)
        with sqlite3.connect(self.new) as c:self.assertEqual(legacy_snapshot(c),audit['legacy_snapshot'])
        q=query_summary(self.new);self.assertEqual(len(q),4);self.assertEqual(q['eligible_minutes']['count'],4)
        self.m['runner_sha256']='d'*64;self.write();c=import_results(self.out,self.new)
        self.assertEqual(c['counts']['pk_components'],24);self.assertEqual(c['counts']['pk_analysis_runs'],8)
        self.assertEqual(query_summary(self.new)['eligible_minutes']['count'],8) # cohorts MUST NOT be mixed
        self.assertEqual(c['counts']['window_metrics'],1)
    def test_no_overwrite_no_inplace_no_foreign_db(self):
        original=sha(self.old)
        with self.assertRaises(ValueError):create_migrated_copy(self.old,self.old)
        self.new.write_bytes(b'USER EXISTING FILE')
        with self.assertRaises(FileExistsError):create_migrated_copy(self.old,self.new)
        self.assertEqual(self.new.read_bytes(),b'USER EXISTING FILE');self.assertEqual(sha(self.old),original)
        with self.assertRaises(ValueError):import_results(self.out,self.old)
        self.assertEqual(sha(self.old),original)
    def test_supplemental_control_registration_is_idempotent(self):
        from pk_catalog import register_extra_controls
        create_migrated_copy(self.old,self.new);import_results(self.out,self.new)
        audit=self.root/'additional.json';audit.write_text(json.dumps([dict(selection='audit_fixture',agreement=False)]))
        a=register_extra_controls(self.new,audit);before=sha(self.new);b=register_extra_controls(self.new,audit)
        self.assertEqual(a,b);self.assertEqual(before,sha(self.new));self.assertEqual(a['controls'],1)
        self.assertTrue(import_results(self.out,self.new)['legacy_logically_preserved'])
    def test_tampered_source_or_export_refused(self):
        create_migrated_copy(self.old,self.new);before=sha(self.new)
        Path(self.m['source']['path']).write_bytes(b'corrupt')
        with self.assertRaises(ValueError):import_results(self.out,self.new)
        self.assertEqual(before,sha(self.new))

if __name__=='__main__':unittest.main(verbosity=2)
