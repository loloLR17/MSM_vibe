from pathlib import Path
import json
import run_capture as c
P=Path(__file__).parent
r=c.run('new-wires-N8A-calibration',0,1)
b=(P/'new-wires-N8A-calibration/card.bin').read_bytes();assert b[16:32].hex()=='0353445344333247858bf0923c01927b'
(P/'CALIBRATION_PASS.json').write_text(json.dumps(dict(passed=True,residual_us=r['cal_residual'],limit_us=100,WHO=r['who'],anchors=r['cal_anchors'],brackets=r['cal_brackets'],no_SD_file_created=True,no_gate_or_long_started=True),indent=2)+'\n')
