from pathlib import Path
import json,re,hashlib,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4r');h=p/'hardware';log=(h/'r4r-trigger11-genuine-r2.log').read_text(errors='replace')
def numbers(line):return {k:int(v) for k,v in re.findall(r'(\w+) = ([0-9]+)(?=[,}])',line)}
lines=log.splitlines();result=numbers([x for x in lines if re.match(r'\$\d+ = \{stage = .*words = ',x)][-1]);bench=numbers([x for x in lines if re.match(r'\$\d+ = \{stage = .*line = ',x)][-1]);workline=[x for x in lines if re.match(r'\$\d+ = \{context = .*work_limit_us',x)][-1];work=numbers(workline)
time=numbers([x for x in lines if re.match(r'\$\d+ = \{fifo = ',x)][-1]);ranges=[]
for at,n,c,stage,status in re.findall(r'\{at = (\d+), bytes = (\d+), commands = (\d+), stage = (\d+), status = (\d+)\}',log):
 if int(n):ranges.append(dict(at=int(at),bytes=int(n),commands=int(c),stage=int(stage),status=int(status)))
a=(h/'fram-before.bin').read_bytes();b=(h/'fram-after-live.bin').read_bytes();audit=json.loads((p/'actual-fram-audit.json').read_text())
assert result['stage']==12 and result['error']==11 and result['valid_responses']==0 and result['first_window_checked']==0
assert work['max_work_us']<=1000 and work['max_service_us']>2500 and bench['max_gap_us']<=5000
assert 'deadline_missed = true' in workline and 'indeterminate = true' in log
assert all(122880<=x['at']<x['at']+x['bytes']<=245760 and x['status']==0 for x in ranges)
assert sum(x['bytes'] for x in ranges)==bench['write_bytes']==104852
assert a[:122880]==b[:122880] and a[245760:]==b[245760:] and len(b)==262144
selected=[c for j in audit['journal'] for c in j['copies'] if c.get('txid')==5];assert max(selected,key=lambda x:x['generation'])['phase']==2
r=dict(outcome='BLOCKED',reason='Measured cooperative service2715us exceeds2500us during genuine START4/5, persistent STARTED/run5; no replay or clearing authorized',physical_code='0e808f3',result=result,bench=bench,work=work,time=time,write_ranges=ranges,range_commands=sum(x['commands'] for x in ranges),historical_and_tail_exact=True,fram_sha256=hashlib.sha256(b).hexdigest(),changed_fram_bytes=sum(x!=y for x,y in zip(a,b)),root_generation=audit['active_root']['generation'],window_oracles='NOT_RUN: only118XYZ/112timestamped, N4096 incomplete; zeros in unused buffers are not measurements',clock_long_run='NOT_VERIFIED: stopped before first100ms check',unpaired_component_maxima='poll2234/publish405/PDU540 are separate maxima, never summed as a measured trace',utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(p/'physical-summary.json').write_text(json.dumps(r,indent=2)+'\n');print('Physical blocked; protected media and persistent STARTED5 audited',flush=True)
