import unittest
import numpy as np
from resolution import quantize
class ResolutionTests(unittest.TestCase):
 def test_quantizer_known_analogue_bound_and_clip(self):
  x=np.linspace(-1500,1500,10001)+.019
  for step in [.244,.122,.061]:
   self.assertLessEqual(max(abs(quantize(x,step)-x)),step/2+1e-12)
   self.assertAlmostEqual(float(quantize(np.array([1e8]),step)[0]),32767*step)
 def test_requantization_cannot_create_information(self):
  analogue=np.array([.01,.02,.03,.04]);coarse=quantize(analogue,.244)
  np.testing.assert_array_equal(quantize(coarse,.061),coarse)
  self.assertTrue(np.any(quantize(analogue,.061)!=coarse))
if __name__=='__main__':unittest.main()
