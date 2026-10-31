"""One-shot trigger only; long mode returns after first active/SD-ACK witness.
The only resident child is a native passive Windows clock instrument.
"""
from pathlib import Path
import json,sys,time,subprocess,datetime
import hardware_second as h
P=Path(__file__).parent;R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R3-8h-faststart');PS='/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe'
def native(p):return Path(subprocess.check_output(['wslpath','-u',p],text=True).strip())
def start(name,previous,run,minimum):
 host=json.loads((P/'native-host.json').read_text(encoding='utf-8-sig'));out=host['Temp']+'\\TR2-R3-N8A-20261031-'+name;dest=native(out)
 assert not dest.exists() and not (P/(name+'-launched.json')).exists()
 args=['python3',str(R/'experiments/r3_time_reference/tools/launch_observer.py'),'--port','COM5','--output',out,'--seconds',str(minimum),'--run',str(run)]
 child=json.loads(subprocess.check_output(args,text=True,timeout=25));(P/(name+'-observer-launch.json')).write_text(json.dumps(child,indent=2)+'\n')
 for _ in range(200):
  if (dest/'ready.json').exists():break
  error=Path(str(dest)+'.stderr.log')
  if error.exists() and error.stat().st_size:raise RuntimeError(error.read_text(errors='replace'))
  time.sleep(.1)
 else:raise RuntimeError('Observer not ready; no trigger submitted')
 body=h.IDENTITY+f'''p r3_result
p r3_clock_fault
if r3_result.version!=4 || r3_result.stage!=4 || r3_result.error || r3_clock_fault || r3_command || fifo.running || r3_writer.active || r3_result.run!={previous} || !(*(unsigned int*)0x42021414 &0x1000)
 quit 4
end
set var r3_command=0x{0x44335200|run:08x}
'''
 h.debug(name+'-trigger-detach',P/'arm/r3_coex.elf',body)
 # Bounded startup only, never an8h Codex monitor. ACTIVE in this firmware
 # implies at least one real SDgroup has completed (sd_written>0).
 first=None
 for _ in range(600):
  if (dest/'summary.json').exists():
   summary=json.loads((dest/'summary.json').read_text(encoding='utf-8-sig'))
   if summary.get('failure'):raise RuntimeError(summary['failure'])
  journal=dest/'queries.jsonl'
  if journal.exists():
   for line in journal.read_text(encoding='utf-8-sig').splitlines():
    try:row=json.loads(line)
    except json.JSONDecodeError:continue
    if row['flags']==(1|(run<<8)):first=row;break
  if first:break
  time.sleep(.2)
 else:raise RuntimeError('No active SD-ACK snapshot; do not assume stopped')
 sys.path.insert(0,str(R/'experiments/r3_time_reference/tools'));from analyze_reference import active_frame
 frame=active_frame(bytes.fromhex(first['frame_hex']));assert frame['accepted']>0
 meta=dict(utc_annotation=datetime.datetime.now(datetime.timezone.utc).isoformat(),code_sha=json.loads((P/'artifact.json').read_text())['code_sha'],run=run,native_observer=child,initial_active_with_SD_group_ack=frame,first_QPC_bracket=first,GDB_detached=True,Codex_must_end_after_launch_checkpoint=(run==5),no_Codex_WSL_monitor=True,no_poweroff_until_postrun_RAM_digest_dump=True)
 (P/(name+'-launched.json')).write_text(json.dumps(meta,indent=2)+'\n');print(json.dumps(meta,indent=2),flush=True)
 if run==5:return # Explicitly NO WAIT/monitor in long mode.
 for _ in range(150):
  if (dest/'summary.json').exists():break
  time.sleep(.2)
 else:raise RuntimeError('Short witness terminal missing; do not repeat')
 summary=json.loads((dest/'summary.json').read_text(encoding='utf-8-sig'));from analyze_reference import active_bounds
 result=active_bounds(summary);result['terminal']=active_frame(bytes.fromhex(summary['terminal']['frame_hex']));assert result['terminal']['flags']==(4|(run<<8))
 (P/(name+'.json')).write_text(json.dumps(summary,indent=2)+'\n');(P/(name+'-analysis.json')).write_text(json.dumps(result,indent=2)+'\n')
 for f in dest.iterdir():
  if f.is_file():(P/(name+'-native-'+f.name)).write_bytes(f.read_bytes())
 print('SHORT PASS',json.dumps(result),flush=True)
if __name__=='__main__':start(sys.argv[1],int(sys.argv[2]),int(sys.argv[3]),int(sys.argv[4]))
