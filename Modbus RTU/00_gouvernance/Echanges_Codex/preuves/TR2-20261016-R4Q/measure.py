from pathlib import Path
import re,json,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-r4q');log=(p/'hardware/r4q-terminal-observation.log').read_text(errors='replace');objects=re.findall(r'\$\d+ = \{([^\n]+)\}',log)
def fields(s):return {k:(v=='true' if v in ('true','false') else int(v)) for k,v in re.findall(r'(\w+) = (\d+|true|false)(?=,|\s*})',s+'}')}
q=fields(objects[0]);v=fields(objects[1]);w=fields(objects[2])
assert q['stage']==10 and q['error']==0 and q['responses']==872 and q['b3_flags']==4097
assert q['end_us']-q['begin_us']>=10000000 and v['write_bytes']==0 and v['xyz']==0
assert w['max_work_us']<=1000 and w['max_service_us']<=2500 and not w['deadline_missed'] and 'error = TR2_OK' in objects[2]
assert all(q[b]==218 for b in ['b0','b3','b4','b5'])
x=dict(result='PASS',q=q,fram=v,work=w,readonly_duration_us=q['end_us']-q['begin_us'],UART_IRQ_TX_measured=False,electrical_RTU_measured=False,physical_FIFO_B3_measured=False,profile_refusal_test=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(p/'measurements.json').write_text(json.dumps(x,indent=2)+'\n');print('PHYSICAL_MEASUREMENTS_PARSED_PASS')
