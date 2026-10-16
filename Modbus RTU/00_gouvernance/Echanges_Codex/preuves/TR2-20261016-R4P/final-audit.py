from pathlib import Path
import json,hashlib,subprocess,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4p');h=p/'hardware'
r=json.loads((p/'measurements.json').read_text());assert r['overall_phase_C_pass']
a=r['physical_mode8'];v=a['r4_result'];w=a['work'];ops=a['*r4m_operations@r4m_op_count']
assert [(x['epoch'],x['txid'],x['code']) for x in ops]==[(2,65535,12),(3,1,13),(3,65535,12),(4,2,14),(4,3,1),(4,4,13)]
assert ops[1]['run_id']==3 and ops[5]['run_id']==4 and a['run60_observed_us']>=60000000
assert a['r4m_old_rejected']==24 and a['r4m_op_count']==v['commands']==6
assert a['r4m_fault_seen_us']>ops[-1]['end_us'] and a['r4m_fault_reaction_us']==a['r4m_stop_end_us']-a['r4m_fault_seen_us']
assert 'error = TR2_ERROR_STORAGE' in (h/'r4p-observation8.log').read_text(errors='replace')
assert not w['deadline_missed'] and 'indeterminate = false' in a['engine'] and 'state = LIVE_FAULT' in a['engine']
assert a['bench_log_refused']==a['bench_preclose_overflow']==0
expected='a13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9'
raw=(h/'fram-after-resume.bin').read_bytes();assert hashlib.sha256(raw).hexdigest()==expected
for n in ['fram-after-reboot.bin','fram-after-restoration.bin']:assert (h/n).read_bytes()==raw
before=(h/'fram-before.bin').read_bytes();assert before[:122880]==raw[:122880] and before[245760:]==raw[245760:]
assert (h/'flash-before.bin').read_bytes()==(h/'flash-restored.bin').read_bytes()
audit=json.loads((p/'actual-final-audit.json').read_text())
def crcs(x):
 if isinstance(x,dict):
  for k,v in x.items():
   if k in ('crc_valid','payload_crc_valid'):assert v is True
   crcs(v)
 elif isinstance(x,list):
  for v in x:crcs(v)
crcs(audit);assert audit['active_root']['generation']==37
for n in ['reboot-summary.json','restoration-summary.json']:assert json.loads((h/n).read_text())['result']=='PASS'
result=dict(result='PASS',checks='six exact R2 commands; 60s; old key rejected; postterminal software fault; causal stop; budgets; complete write/CS/protected audit; metadata CRC and exact full snapshots; no new indeterminate; no auto-start; Flash restored without FRAM rewind',different_fram_bytes=sum(x!=y for x,y in zip(before,raw)),utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(p/'final-audit.json').write_text(json.dumps(result,indent=2)+'\n');print(result)
