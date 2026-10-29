import importlib.util,unittest
from pathlib import Path
p=Path(__file__).resolve().parents[1]/'tools/analyze_capture.py';spec=importlib.util.spec_from_file_location('analyze',p);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
class Guard(unittest.TestCase):
 def test_exact_then_corruption_or_truncation(self):
  b=m.expected(1001,0x52325333)+b'\xff'*23;self.assertTrue(m.integrity(b,1001,0x52325333)['all_bytes_exact'])
  for bad in [b[:-1],bytes([b[0]^1])+b[1:],b[:-1]+b'\0']:
   with self.assertRaises(ValueError):m.integrity(bad,1001,0x52325333)
 def test_abi_and_nearest_rank(self):
  with self.assertRaises(ValueError):m.result(b'\0'*232)
  self.assertEqual(m.stats(list(range(1,101)))['p95'],95)
  self.assertEqual(m.stats([1,2,3])['p99'],3)
  self.assertEqual(m.expected(4,0),b'\0'*4)
if __name__=='__main__':unittest.main()
