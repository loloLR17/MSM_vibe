#!/usr/bin/env python3
"""Generate an explicitly instrumented build copy. Never write the ST SDK source."""
from pathlib import Path
import argparse
import hashlib
import json

EXPECTED_SHA = 'a1668346fe2847e5ed1fb1c7812158e3e39391a62cfd6487c4973e66308c22a3'
INCLUDE = '#include "stm32u5xx_hal.h"'
MARKER = 'static void SPI_CloseTransfer(SPI_HandleTypeDef *hspi)\n{'

def adapt(source):
    if source.count(INCLUDE)!=1 or source.count(MARKER)!=1:
        raise ValueError('HAL source shape changed; no approximate patch allowed')
    source=source.replace(INCLUDE, INCLUDE+'\nextern void bench_spi_preclose(SPI_HandleTypeDef *hspi);',1)
    return source.replace(MARKER,MARKER+'\n  bench_spi_preclose(hspi); /* M015-M observer, before clearing flags/counts */',1)

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('source',type=Path);parser.add_argument('output',type=Path)
    args=parser.parse_args();original=args.source.read_bytes()
    if hashlib.sha256(original).hexdigest()!=EXPECTED_SHA:
        parser.error('Expected qualified CubeU5 v1.9.0 HAL SPI exact source')
    if args.source.resolve()==args.output.resolve():parser.error('SDK source must remain unchanged')
    args.output.parent.mkdir(parents=True,exist_ok=True)
    generated=adapt(original.decode().replace('\r\n','\n')).encode()
    args.output.write_bytes(generated)
    manifest={'source':str(args.source),'original_sha256':EXPECTED_SHA,
              'build_copy':str(args.output),'generated_sha256':hashlib.sha256(generated).hexdigest(),
              'changes':'LF normalization + observer declaration + one preclose call; original ST algorithm unchanged'}
    args.output.with_suffix('.json').write_text(json.dumps(manifest,indent=2)+'\n')

if __name__=='__main__':main()
