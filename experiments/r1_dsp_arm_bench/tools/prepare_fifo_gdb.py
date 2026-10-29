#!/usr/bin/env python3
"""Prepare only. One terminal hardware breakpoint, never in calibration/drain."""
import argparse,json,subprocess
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--directory',type=Path,required=True);a=p.parse_args();a.directory.mkdir(parents=True,exist_ok=False)
identity='''x/3wx 0x0BFA0700
if *(unsigned int*)0x0BFA0700 != 0x001B0046 || *(unsigned int*)0x0BFA0704 != 0x38425018 || *(unsigned int*)0x0BFA0708 != 0x20333157
 detach
 quit 3
end
'''
for run in range(1,5):
 d=a.directory/('run'+str(run));d.mkdir()
 def path(n):
  s=subprocess.check_output(['wslpath','-w',str((d/n).resolve())],text=True).strip().replace('\\','/')
  if any(c.isspace() or c=='"' for c in s):raise ValueError('Spaceless GDB filename required')
  return s
 body=identity+'''p rf_result
if rf_result.error || fifo.running || rf_command != 0
 detach
 quit 4
end
'''
 body+=('if rf_result.stage != 2 || rf_result.run != 0\n' if run==1 else f'if rf_result.stage != 4 || rf_result.run != {run-1} || !fifo.stopped\n')+' detach\n quit 5\nend\n'
 body+=f'thbreak rf_finished\nset var rf_command=0x{0x52314600|run:08x}\ncontinue\np rf_result\np fifo\np fir\n'
 # All reads are after the terminal hardware breakpoint, including failure.
 for name,start,end in [('result','&rf_result','(&rf_result+1)'),('records','rf_records','(rf_records+rf_result.records)'),('brackets','rf_brackets','(rf_brackets+rf_result.cal_brackets)'),('raw','rf_raw','(rf_raw+rf_result.raw_words*7)'),('sources','rf_sources','(rf_sources+rf_source_count)'),('outputs','rf_outputs','(rf_outputs+rf_output_count)')]:
  file=d/(name+'.bin');file.write_bytes(b'')
  body+=f'if {end} > {start}\ndump binary memory {path(name+".bin")} {start} {end}\nend\n'
 body+='if rf_result.stage != 4 || rf_result.error || fifo.running || !fifo.stopped\n detach\n quit 6\nend\n'
 (d/'body.gdb').write_text(body)
(a.directory/'manifest.json').write_text(json.dumps(dict(prepared_only=True,run1='WHO plus2s actual clock calibration',run2='2s bypass',run3='2s FIR',run4='10s FIR only after successful short pair',probe='0032001F3235510C37333439',no_reset_or_load=True),indent=2)+'\n')
