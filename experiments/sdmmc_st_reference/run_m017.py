"""One immutable M017 run; capture terminal only, never halt a running campaign.
Use the qualified hardware.py stored with the mission's local evidence.
A pre-existing marker is a refusal, never an automatic retry.
"""
from pathlib import Path
import argparse,datetime,hashlib,json,re,struct,sys,time
parser=argparse.ArgumentParser()
parser.add_argument('--proof',required=True,type=Path)
parser.add_argument('--elf',required=True,type=Path)
parser.add_argument('--code-sha',required=True)
parser.add_argument('--run',required=True,type=int,choices=(1,2,3,4))
a=parser.parse_args(); P=a.proof; sys.path.insert(0,str(P)); from hardware import debug,IDENTITY,win
name=f'run-{a.run:02d}'
marker=P/(name+'-start.json')
assert not marker.exists(), 'Run already started: preserve evidence, no implicit retry'
marker.write_text(json.dumps({'run':a.run,'source_sha':a.code_sha,'bin_sha256':hashlib.sha256(a.elf.with_suffix('.bin').read_bytes()).hexdigest(),'utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'parameters':'600s;426672B/s;1bit10MHz;ring131072;chunk65536;LBA2048'},indent=2)+'\n')
fields=['load_stage','load_error','load_phase','load_transfer_phase','load_generated','load_produced','load_consumed','load_backlog_max','load_overflow','load_lost','load_producer_done','load_producer_enabled','load_written','load_written_logical','load_verified','load_verified_logical','load_write_chunks','load_read_chunks','load_mismatches','load_first_mismatch','load_first_io_error','load_first_cmd_error','load_max_producer_gap_us','load_generation_us','load_write_elapsed_us','load_verify_elapsed_us','load_clock_hz','load_kernel_hz','load_clkcr','load_bus_card','load_blocks','load_dma_error','load_hal_return','load_last_r1','load_cmd_error','load_producer_cycles','load_producer_max_cycles']
body=IDENTITY+'''if load_stage != 2 || load_error != 0 || load_generated != 0 || load_written != 0 || load_trigger != 0
 quit 4
end
hbreak load_terminal
set load_trigger=0x4C314C44
continue
'''
for f in fields: body+=f'printf "METRIC {f}=%u\\n", {f}\n'
body+='p/x load_cid\np/x hsd2.ErrorCode\nx/2wx 0xE000ED28\n'
for field,label in [('load_write_latency_us','total'),('load_dma_latency_us','data'),('load_ready_latency_us','ready')]:
 body+='dump binary memory '+win(P/(name+'-'+label+'.bin'))+f' {field} {field}+4096\n'
start=time.monotonic();print(name,'RUN started; no intermediate halt',flush=True)
debug(name,a.elf,body,timeout=1800)
m={k:int(v) for k,v in re.findall(r'METRIC (\w+)=(\d+)',(P/(name+'.log')).read_text())}
lat={k:list(struct.unpack('<4096I',(P/(name+'-'+k+'.bin')).read_bytes()))[:m['load_write_chunks']] for k in ['total','data','ready']}
result={'metrics':m,'latency_us':lat,'host_elapsed_s_including_connection':time.monotonic()-start,'utc_completed':datetime.datetime.now(datetime.timezone.utc).isoformat(),'passed':m['load_stage']==5 and m['load_generated']==256003200 and m['load_written']==256003584 and m['load_verified']==256003584 and m['load_lost']==0 and m['load_overflow']==0 and m['load_mismatches']==0}
(P/(name+'-result.json')).write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result),flush=True)
