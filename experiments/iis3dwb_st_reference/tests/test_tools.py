import importlib.util
from pathlib import Path
import subprocess
import sys
import unittest
ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('decoder', ROOT/'tools/decode_capture.py')
decoder = importlib.util.module_from_spec(spec); spec.loader.exec_module(decoder)

class Tools(unittest.TestCase):
    def test_raw_decode_and_rebuild(self):
        raw = bytes.fromhex('10ffff0080ff7f20010000000000')
        rows = decoder.decode(raw)
        self.assertEqual((rows[0]['x'],rows[0]['y'],rows[0]['z']),(-1,-32768,32767))
        self.assertEqual(rows[1]['timestamp_ticks'],1)
        self.assertEqual(b''.join(bytes.fromhex(r['raw_hex']) for r in rows),raw)
        self.assertTrue(all(not r['accepted'] for r in rows))
    def test_bounds(self):
        for raw in [b'123',bytes(513*7)]:
            with self.assertRaises(ValueError): decoder.decode(raw)
    def test_primary_refused(self):
        result = subprocess.run([sys.executable,str(ROOT/'tools/plan_target.py'),
          '--second-sn','003500463235510B37333439','--second-uid','111111111111111111111111',
          '--wiring-confirmed'],capture_output=True)
        self.assertNotEqual(result.returncode,0)
    def test_no_wiring_refused(self):
        result = subprocess.run([sys.executable,str(ROOT/'tools/plan_target.py'),
          '--second-sn','222222222222222222222222','--second-uid','111111111111111111111111'],capture_output=True)
        self.assertNotEqual(result.returncode,0)
    def test_plan_only(self):
        result = subprocess.run([sys.executable,str(ROOT/'tools/plan_target.py'),
          '--second-sn','222222222222222222222222','--second-uid','111111111111111111111111',
          '--wiring-confirmed'],capture_output=True)
        self.assertEqual(result.returncode,0)
        self.assertIn(b'PLAN_ONLY_NO_EXECUTION',result.stdout)

if __name__ == '__main__': unittest.main()
