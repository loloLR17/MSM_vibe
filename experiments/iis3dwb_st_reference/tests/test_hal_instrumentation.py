import importlib.util
from pathlib import Path
import unittest
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('instrument',ROOT/'tools/instrument_hal.py')
mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
class Instrumentation(unittest.TestCase):
    def test_exact_only_delta(self):
        original=mod.INCLUDE+'\n'+mod.MARKER+'\n  clear_flags();\n  reset_counts();\n}\n'
        adapted=mod.adapt(original)
        self.assertEqual(adapted.count('bench_spi_preclose(hspi);'),1)
        self.assertLess(adapted.index('bench_spi_preclose(hspi);'),adapted.index('clear_flags();'))
        reverted=adapted.replace('\nextern void bench_spi_preclose(SPI_HandleTypeDef *hspi);','').replace('\n  bench_spi_preclose(hspi); /* M015-M observer, before clearing flags/counts */','')
        self.assertEqual(reverted,original)
    def test_changed_shape_refused(self):
        for source in ['',mod.INCLUDE,mod.INCLUDE+'\n'+mod.MARKER+mod.MARKER]:
            with self.assertRaises(ValueError):mod.adapt(source)
if __name__=='__main__':unittest.main()
