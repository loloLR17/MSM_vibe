"""Offline parser/generator guards only; no target connection or timing claim."""
import ctypes,importlib.util,pathlib,subprocess,sys,tempfile,unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('capture',ROOT/'tools/analyze_capture.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
class Guards(unittest.TestCase):
 def test_schema_and_nearest_rank(self):
  self.assertEqual(ctypes.sizeof(m.Result),1136)
  s=m.Stats(count=5,minimum=1,p95=99,p99=99,maximum=99,sum=113)
  self.assertEqual(m.checked_stats([99,1,7,4,2],s)['mean'],22.6)
  s.sum=112
  with self.assertRaises(ValueError):m.checked_stats([99,1,7,4,2],s)
 def test_nonterminal_capture_never_becomes_a_result(self):
  with tempfile.TemporaryDirectory() as d:
   p=pathlib.Path(d);r=m.Result();r.uid[:]=[0x001B0046,0x38425018,0x20333157];r.clock_hz=160000000;r.stage=2;r.version=1
   (p/'result.bin').write_bytes(bytes(r))
   for name,size in [('blocks.bin',8*256*4),('emissions.bin',8*4096*4),('overhead.bin',256*4)]: (p/name).write_bytes(bytes(size))
   z=subprocess.run([sys.executable,str(ROOT/'tools/analyze_capture.py'),str(p),str(p/'no-result.json')],capture_output=True,text=True)
   self.assertNotEqual(z.returncode,0);self.assertIn('Not a successful DONE capture',z.stderr);self.assertFalse((p/'no-result.json').exists())
 def test_dump_paths_are_unquoted_and_no_reset_load(self):
  with tempfile.TemporaryDirectory() as d:
   p=pathlib.Path(d);(p/'dummy.bin').write_bytes(bytes(32))
   z=subprocess.run([sys.executable,str(ROOT/'tools/prepare_gdb.py'),'--image',str(p/'dummy.bin'),'--scripts',str(p/'scripts'),'--capture',str(p/'capture')],capture_output=True,text=True)
   self.assertEqual(z.returncode,0,z.stderr);s=(p/'scripts/run-to-terminal.gdb').read_text()
   self.assertIn('thbreak r1_bench_finished',s);self.assertIn('stage != 4',s);self.assertNotIn('monitor reset',s);self.assertNotIn('\nload',s)
   for line in s.splitlines():
    if line.startswith('dump binary memory '):self.assertNotIn('"',line)
if __name__=='__main__':unittest.main()
