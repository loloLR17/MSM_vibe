"""Synthetic offline guard tests; never ARM timing or sensor qualification."""
import importlib.util,struct,tempfile,unittest
from pathlib import Path
P=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('fifo_tools',P/'tools/analyze_fifo_capture.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
class Tools(unittest.TestCase):
 def fixture(self,d):
  r=dict.fromkeys(m.FIELDS,0);r.update(version=1,stage=4,uid0=0x1b0046,uid1=0x38425018,uid2=0x20333157,clock=160000000,run=3,mode=1,stopped=1,duration_us=1000,records=1)
  (d/'result.bin').write_bytes(struct.pack('<'+'I'*len(r),*[r[k] for k in m.FIELDS]));(d/'records.bin').write_bytes(struct.pack('<IIII',1600,800,40000,1))
  (d/'sources.bin').write_bytes(struct.pack('<QQhhh??',0,1234567,1000,0,0,True,True));(d/'outputs.bin').write_bytes(struct.pack('<QQQQhhhBBB7x',3,0,0,1234567,0,0,0,1,0,0))
  (d/'raw.bin').write_bytes(struct.pack('<Bhhh',16,1000,0,0))
 def test_prefix_replay_and_real_raw(self):
  with tempfile.TemporaryDirectory() as n:
   d=Path(n);self.fixture(d);x=m.analyze(d);self.assertEqual(x['independent_fir_outputs'],1);self.assertEqual(x['distributions']['service_cycles']['maximum'],1600)
   (d/'raw.bin').write_bytes(struct.pack('<Bhhh',16,999,0,0))
   with self.assertRaisesRegex(ValueError,'XYZ disagreement'):m.analyze(d)
 def test_wrong_output_and_abi(self):
  with tempfile.TemporaryDirectory() as n:
   d=Path(n);self.fixture(d);(d/'outputs.bin').write_bytes(struct.pack('<QQQQhhhBBB7x',3,0,0,1234567,1,0,0,1,0,0))
   with self.assertRaisesRegex(ValueError,'replay disagreement'):m.analyze(d)
   with self.assertRaises(ValueError):m.unpack_result(b'\0'*12)
if __name__=='__main__':unittest.main()
