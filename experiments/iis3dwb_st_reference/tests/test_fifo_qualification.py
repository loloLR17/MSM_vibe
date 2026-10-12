import sys,unittest,struct
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from qualify_fifo import analyze

def word(tag,counter,payload):
    b=(tag<<3)|(counter<<1);b|=b.bit_count()%2
    return bytes([b])+payload
X=lambda c:word(2,c,struct.pack('<hhh',100+c,-50,4000))
T=lambda t:word(4,0,struct.pack('<I',t)+b'\0\0')
class Qualification(unittest.TestCase):
    def check(self,data,**options):
        kw=dict(expected_entries=len(data)//7,result=0,cleanup=0,transfers=[dict(status=0,error=0,valid=1)],statuses=[0x81],timestamp_decimation=0);kw.update(options);return analyze(data,**kw)
    def test_mixed_timestamp_ratio(self):
        data=T(20)+b''.join(X(i%4) for i in range(8))+T(44)
        self.assertTrue(self.check(data,timestamp_decimation=8)['bounded_acceptance'])
    def test_timestamp_only(self):self.assertIn('no_XYZ',self.check(T(20)+T(44))['reasons'])
    def test_partial(self):self.assertIn('partial_window',self.check(X(0),expected_entries=512)['reasons'])
    def test_spi(self):self.assertIn('SPI_refusal',self.check(X(0),transfers=[dict(status=3,error=260,valid=0)])['reasons'])
    def test_counter_wrap(self):self.assertTrue(self.check(X(3)+X(0)+X(1))['bounded_acceptance'])
    def test_counter_gap(self):self.assertIn('counter_discontinuity',self.check(X(0)+X(2))['reasons'])
    def test_fifo_overflow(self):self.assertIn('FIFO_overflow_or_missing_status',self.check(X(0),statuses=[0xC1])['reasons'])
    def test_cleanup_impossible(self):self.assertIn('cleanup_failed',self.check(X(0),cleanup=1)['reasons'])
    def test_parity(self):
        data=bytes([X(0)[0]^1])+X(0)[1:];self.assertIn('tag_parity',self.check(data)['reasons'])
    def test_time_backwards(self):self.assertIn('timestamp_nonmonotonic',self.check(X(0)+T(44)+T(20))['reasons'])
    def test_time_wrap(self):self.assertTrue(self.check(X(0)+T(0xFFFFFFF0)+T(8))['bounded_acceptance'])
    def test_bad_ratio(self):self.assertIn('timestamp_XYZ_ratio',self.check(T(20)+X(0)+T(44),timestamp_decimation=8)['reasons'])
    def test_truncation(self):self.assertIn('logging_capacity',self.check(X(0),trace_refused=1)['reasons'])
    def test_incomplete_word(self):
        with self.assertRaises(ValueError):self.check(b'123')
if __name__=='__main__':unittest.main()
