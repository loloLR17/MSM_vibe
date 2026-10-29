"""Frequency-axis sensitivity of the deployed Q15 coefficients; no rate measurement.
C tests separately pass externally observed unwrapped times at these rational rates.
"""
from pathlib import Path
import json,sys
import numpy as np
from scipy import signal
c=np.array(json.loads((Path(__file__).parent/'decim10_coefficients.json').read_text())['q15'])/32768
rows=[]
for ppm in [-1000,-50,0,50,1000]:
 fs=(80000/3)*(1+ppm/1e6);f=np.linspace(fs/10-700,fs/2,262145)
 _,z=signal.freqz(c,worN=f,fs=fs);slope=2*np.pi/fs*np.sum(abs(np.arange(107)-53)*abs(c));bound=max(abs(z))+slope*(f[1]-f[0])/2+1e-12
 rounding=2*(.5*sum(abs(c))+.5)/12000
 _,g=signal.freqz(c,worN=np.linspace(0,700,16385),fs=fs)
 row=dict(ppm=ppm,input_hz=fs,output_hz=fs/10,passband_error_db=float(max(abs(20*np.log10(abs(g))))),continuous_stop_bound_db=float(20*np.log10(bound)),tone_bound_with_quantization_db=float(20*np.log10(bound+rounding)))
 assert row['passband_error_db']<.1 and row['tone_bound_with_quantization_db']< -60
 rows.append(row)
Path(sys.argv[1]).write_text(json.dumps(dict(passed=True,source_rate='provided externally, not inferred or measured by this component',rates=rows),indent=2)+'\n');print(rows)
