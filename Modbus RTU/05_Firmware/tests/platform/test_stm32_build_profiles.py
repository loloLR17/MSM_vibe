"""A04-01 configuration contracts; no board access, no mocks of CMake guards.

Run with STM32CUBE_U5_ROOT and optionally --evidence-dir <fresh/preserved dir>.
Each case uses a separate build directory and checks the specific refusal.
"""
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

FW = Path(__file__).resolve().parents[2]
SRC = FW / 'platform/stm32'
REPO = FW.parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--evidence-dir', type=Path)
args, remaining = parser.parse_known_args()
EVIDENCE = args.evidence_dir or Path(tempfile.mkdtemp(prefix='tr2-build-profiles-'))
EVIDENCE.mkdir(parents=True, exist_ok=True)
RESULTS = []

class Profiles(unittest.TestCase):
    def configure(self, name, flags, refusal=None, label=None):
        sdk = os.environ['STM32CUBE_U5_ROOT']
        command = ['cmake', '-G', 'Ninja', '-S', str(SRC), '-B', str(EVIDENCE/name),
                   '-DCMAKE_TOOLCHAIN_FILE='+str(SRC/'cmake/arm-none-eabi-gcc.cmake'),
                   '-DSTM32CUBE_U5_ROOT='+sdk,
                   '-DTR2_COMMISSIONING_MANIFEST='+str(REPO/'Modbus RTU/00_gouvernance/COMMISSIONING_M014_H.json'),
                   '-DTR2_COMMISSIONING_REGISTRY='+str(REPO/'Modbus RTU/00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json')]
        command += ['-DTR2_STM32_'+flag for flag in flags]
        result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        log_name = f'{len(RESULTS):02d}-{name}.log'
        (EVIDENCE/log_name).write_text(result.stdout)
        RESULTS.append(dict(case=name, command=command, return_code=result.returncode,
                            expected_refusal=refusal, expected_artifact=label, log=log_name))
        if refusal:
            self.assertNotEqual(result.returncode, 0, name)
            self.assertIn(refusal, ' '.join(result.stdout.split()), name)
        else:
            self.assertEqual(result.returncode, 0, result.stdout)
            self.assertIn('artifact='+label+'\n', (EVIDENCE/name/'build-profile.txt').read_text())
        return EVIDENCE/name

    def test_default_and_ambiguous_refused(self):
        self.configure('default', [], 'implicit generic SD harness is forbidden')
        self.configure('unknown-recipe', ['BUILD_PROFILE=unknown'], 'Unknown TR2_STM32_BUILD_PROFILE')
        self.configure('legacy-production', ['LEGACY_SD_HARNESS=ON','PRODUCTION=ON','RS485_TX_QUALIFIED=ON'], 'LEGACY_SD_HARNESS is exclusive')

    def test_cumulative_recipe_and_cache_conflict(self):
        self.configure('cumulative', ['BUILD_PROFILE=CUMULATIVE'], label='tr2-cumulative-m014-m015-spi4')
        self.configure('cumulative-off', ['BUILD_PROFILE=CUMULATIVE','PRODUCTION=OFF'], 'CUMULATIVE conflicts')
        self.configure('cumulative', ['BUILD_PROFILE=CUMULATIVE','IRQ_DIAGNOSTICS=OFF'], 'CUMULATIVE conflicts')

    def test_special_and_legacy_labels(self):
        self.configure('qualification-U', ['ST_FIFO_WINDOW=ON'], label='tr2-special-st_fifo_window-NON-CUMULATIVE')
        self.configure('legacy', ['LEGACY_SD_HARNESS=ON'], label='tr2-legacy-SD-WRITES-NON-PRODUCTION')

    def test_existing_eight_specific_guards(self):
        production = ['PRODUCTION=ON','RS485_TX_QUALIFIED=ON']
        coop = production+['FIFO_COOPERATIVE=ON','IRQ_DIAGNOSTICS=ON']
        cases = [
            ('production-no-TX', ['PRODUCTION=ON'], 'Production requires qualified RS485'),
            ('cooperative-no-production', ['FIFO_COOPERATIVE=ON'], 'Cooperative FIFO requires'),
            ('cooperative-no-observer', production+['FIFO_COOPERATIVE=ON'], 'Cooperative FIFO requires'),
            ('production-manufacturing', production+['MANUFACTURING=ON'], 'are exclusive'),
            ('production-probe', production+['COMMISSIONING_READONLY=ON'], 'are exclusive'),
            ('cooperative-U', coop+['ST_FIFO_WINDOW=ON'], 'Cooperative FIFO requires'),
            ('cooperative-silence', coop+['PRODUCTION_SILENCE_TEST=ON'], 'Cooperative FIFO requires'),
            ('SPI16', ['FIFO_SPI_MHZ=16'], 'bounded to4/8MHz')]
        for name, flags, message in cases:
            with self.subTest(name=name):
                self.configure(name, flags, message)

    def test_helper_requires_explicit_profile(self):
        command = ['bash', str(SRC/'tr2_build_stm32.sh')]
        result = subprocess.run(command, capture_output=True, text=True)
        self.assertEqual(result.returncode, 2)
        self.assertIn('explicit --profile', result.stderr)
        RESULTS.append(dict(case='helper-no-profile',command=command,return_code=result.returncode))

if __name__ == '__main__':
    try:
        unittest.main(argv=[__file__]+remaining)
    finally:
        (EVIDENCE/'commands-results.json').write_text(json.dumps(RESULTS, indent=2)+'\n')
