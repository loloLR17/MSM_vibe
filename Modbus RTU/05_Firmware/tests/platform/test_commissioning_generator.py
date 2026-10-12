import importlib.util
import json
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
spec = importlib.util.spec_from_file_location('generator', ROOT/'Modbus RTU/05_Firmware/platform/stm32/generate_commissioning_identity.py')
generator = importlib.util.module_from_spec(spec); spec.loader.exec_module(generator)
class GeneratorTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(); self.addCleanup(self.temp.cleanup)
        self.p = Path(self.temp.name)
        self.m = json.loads((ROOT/'Modbus RTU/00_gouvernance/COMMISSIONING_M014_H.json').read_text())
        self.r = json.loads((ROOT/'Modbus RTU/00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json').read_text())
    def run_generator(self):
        (self.p/'m.json').write_text(json.dumps(self.m)); (self.p/'r.json').write_text(json.dumps(self.r))
        generator.generate(self.p/'m.json', self.p/'r.json', self.p/'output')
    def test_authoritative_b0(self):
        self.run_generator(); b = json.loads((self.p/'output/expected-b0.json').read_text())
        self.assertEqual([0,1,1,0,14,8,1,12],b[:8]); self.assertEqual(21,len(b)); self.assertEqual(0,b[20])
        self.assertIn('20333157',format(int(self.r['entries'][0]['hardware_uid_words_hex'][2],16),'08X'))
    def test_duplicate_full_uid_refused(self):
        self.r['entries'].append(dict(self.r['entries'][0],device_id=2))
        with self.assertRaisesRegex(ValueError,'Duplicate'): self.run_generator()
    def test_sd_authority_refused(self):
        self.m['storage_policy']['availability']=True
        with self.assertRaisesRegex(ValueError,'No SD'): self.run_generator()
    def test_unqualified_capability_refused(self):
        self.m['b0']['capabilities']=15
        with self.assertRaisesRegex(ValueError,'supervision'): self.run_generator()
    def test_unreviewed_generation_refused(self):
        self.m['provisioning']['expected_generation']=11
        with self.assertRaisesRegex(ValueError,'generation'): self.run_generator()
    def test_reserved_assignment_refused(self):
        self.r['entries'][0]['status']='RESERVED'
        with self.assertRaisesRegex(ValueError,'assigned'): self.run_generator()
if __name__=='__main__': unittest.main()
