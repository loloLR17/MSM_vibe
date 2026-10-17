from pathlib import Path
import json,re,struct,hashlib,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4v');v=json.loads((p/'paced-summary.json').read_text());b=json.loads((p/'baseline-summary.json').read_text());raw=(p/'hardware/paced-phase.log').read_text(errors='replace');nums=lambda s:{k:int(x) for k,x in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',s)}
s=[nums(x) for x in re.findall(r'\{begin_us = [^}]+\}',raw) if 'begin_us = 0,' not in x];ref=next(x for x in s if x['tx']==8);e=next(x for x in v['events'] if x['kind']==9)
assert ref['journal_before']==ref['journal_after']==58 and ref['root_before']==ref['root_after']==71 and ref['running_before']==ref['running_after']==1 and ref['io_us']==0
assert e['tx']==8 and e['result']==7 and e['running']==1 and v['stats']['refused_once']==1
held=False;count=0
for line in (p/'hardware/paced-master.jsonl').read_text().splitlines():
 row=json.loads(line)
 if row['kind']!='adu':continue
 q=bytes.fromhex(row['request']);a=bytes.fromhex(row['response'])
 if q[1]==3 and q[2:4]==b'\x0b\xb8':
  regs=struct.unpack('>48H',a[3:-2])
  if regs[1]&512:held=True
  if held:assert not regs[1]&4;count+=1
assert count==51
rest=json.loads((p/'hardware/restoration-summary.json').read_text());assert rest['pages']==19 and rest['flash_exact_initial'] and rest['fram_exact_last_physical']
assert (p/'hardware/flash-restored.bin').read_bytes()==(p/'hardware/flash-before.bin').read_bytes()
assert (p/'hardware/fram-after-restoration.bin').read_bytes()==(p/'hardware/paced-fram.bin').read_bytes()==(p/'model-paced.bin').read_bytes()
assert '0x7b\t0x00\t0x80\t0x00' in (p/'hardware/restored-sensor-stopped-readonly.log').read_text(errors='replace')
out={'result':'PASS','refusal_span':ref,'refusal_event':e,'no_extra_commits_full_fram_exact_model':v['full_fram_exact_model'],'all_51_held_reads_without_fresh':True,'baseline_code':'f8bf7ca80a7dad385dd84d00ffecea7e3dea125b','candidate_code':'8db2f673ac46c3e4d2bb64f0bbd39ca16bd39a4e','terminal_reduction_percent':100*(1-v['commands'][1]['capture_to_terminal_us']/b['commands'][1]['capture_to_terminal_us']),'host_ack_held_reduction_percent':100*(1-v['host_stop_ack_to_held_s']/b['host_stop_ack_to_held_s']),'restoration_full_flash_and_fram_verified':True,'sensor_stopped_readback_verified':True,'utc':datetime.datetime.now(datetime.timezone.utc).isoformat()};(p/'comparison-final.json').write_text(json.dumps(out,indent=2)+'\n');print(out)
