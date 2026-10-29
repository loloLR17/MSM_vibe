"""Decode ONLY a post-DONE RAM capture. It never launches GDB or a target.
No result is a physical proof without the corresponding guarded GDB invocation.
"""
from pathlib import Path
import argparse,ctypes as C,json,math,hashlib
class Stats(C.LittleEndianStructure):
    _fields_=[(k,C.c_uint32) for k in ['count','minimum','p95','p99','maximum','reserved']]+[('sum',C.c_uint64)]
class Case(C.LittleEndianStructure):
    _fields_=[(k,C.c_uint32) for k in ['fir_enabled','burst','blocks','sources','outputs','input_hash','output_hash','error','irq_before','irq_after','elapsed_cycles','elapsed_ms']]+[('block',Stats),('emission',Stats)]+[(k,C.c_uint32) for k in ['warmup_outputs','clipped_outputs','source_rail_outputs','final_active']]
class Result(C.LittleEndianStructure):
    _fields_=[(k,C.c_uint32) for k in ['version','stage','error','clock_hz']]+[('uid',C.c_uint32*3)]+[(k,C.c_uint32) for k in ['primask','basepri','faultmask','demcr','dwt_ctrl','flash_acr','systick_count','stack_begin','stack_end','stack_untouched','stack_deepest','complete_magic','reserved']]+[('overhead',Stats),('cases',Case*8)]
assert C.sizeof(Stats)==32 and C.sizeof(Case)==128 and C.sizeof(Result)==1136

def checked_stats(values,s):
    if not values:raise ValueError('Empty measured distribution')
    v=sorted(values);n=len(v)
    expected=dict(count=n,minimum=v[0],maximum=v[-1],p95=v[math.ceil(n*.95)-1],p99=v[math.ceil(n*.99)-1],sum=sum(v))
    if any(getattr(s,k)!=value for k,value in expected.items()):raise ValueError('Target statistics disagree with captured raw cycles')
    return dict(**expected,mean=expected['sum']/n)
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('capture',type=Path);ap.add_argument('output',type=Path);a=ap.parse_args()
    files={n:(a.capture/n).read_bytes() for n in ['result.bin','blocks.bin','emissions.bin','overhead.bin']}
    if len(files['result.bin'])!=1136:raise ValueError('Result ABI/length mismatch')
    if len(files['blocks.bin'])!=8*256*4 or len(files['emissions.bin'])!=8*4096*4 or len(files['overhead.bin'])!=256*4:raise ValueError('Cycle-array length mismatch')
    r=Result.from_buffer_copy(files['result.bin'])
    if list(r.uid)!=[0x001B0046,0x38425018,0x20333157] or r.clock_hz!=160000000:raise ValueError('Wrong board/clock')
    if r.version!=1 or r.stage!=4 or r.error or r.complete_magic!=0x4431304F:raise ValueError('Not a successful DONE capture')
    if r.primask or r.basepri or r.faultmask:raise ValueError('Interrupts were masked')
    if not r.stack_untouched:raise ValueError('No intact stack floor marker')
    blocks=(C.c_uint32*(8*256)).from_buffer_copy(files['blocks.bin']);emits=(C.c_uint32*(8*4096)).from_buffer_copy(files['emissions.bin']);overhead=(C.c_uint32*256).from_buffer_copy(files['overhead.bin'])
    rows=[]
    for i,c in enumerate(r.cases):
        if c.fir_enabled!=i%2 or c.burst!=[1,8,60,128][i//2] or c.blocks!=256 or c.sources!=c.burst*256 or c.outputs!=(c.sources+9)//10 or c.error or c.final_active:raise ValueError('Case shape/count/lifecycle mismatch')
        if c.warmup_outputs!=(11 if c.fir_enabled else 0) or c.clipped_outputs or c.source_rail_outputs:raise ValueError('Unexpected synthetic-data quality')
        bs=checked_stats(list(blocks[i*256:i*256+256]),c.block);es=checked_stats(list(emits[i*4096:i*4096+c.outputs]),c.emission)
        rows.append(dict(slot=i,fir_enabled=bool(c.fir_enabled),burst=c.burst,sources=c.sources,outputs=c.outputs,input_hash=c.input_hash,output_hash=c.output_hash,block_cycles=bs,emission_cycles=es,irq_delta=(c.irq_after-c.irq_before)&0xffffffff,instrumented_cycles_per_source=bs['sum']/c.sources,instrumented_cpu_percent_at_nominal_source=bs['sum']/c.sources*(80000/3)/r.clock_hz*100,max_block_us_at_configured_clock=bs['maximum']*1e6/r.clock_hz,elapsed_cycles=c.elapsed_cycles,elapsed_ms=c.elapsed_ms))
    paired=[]
    for i in range(0,8,2):
        x,y=rows[i:i+2]
        if x['input_hash']!=y['input_hash']:raise ValueError('Paired synthetic inputs differ')
        paired.append(dict(burst=x['burst'],added_instrumented_cycles_per_source=y['instrumented_cycles_per_source']-x['instrumented_cycles_per_source'],added_instrumented_cpu_percent=y['instrumented_cpu_percent_at_nominal_source']-x['instrumented_cpu_percent_at_nominal_source']))
    report=dict(source='Post-DONE RAM capture; physical provenance requires guarded invocation and UID/image evidence',synthetic_data=True,fifo_tested=False,rows=rows,paired=paired,read_overhead=checked_stats(list(overhead),r.overhead),clock_hz_configured=r.clock_hz,stack_marker=dict(begin=r.stack_begin,end=r.stack_end,untouched=r.stack_untouched,deepest=r.stack_deepest,scope='after paint, includes WAIT/bench/statistics/IRQs; excludes startup and a 256B live-stack guard'),sha256={k:hashlib.sha256(v).hexdigest() for k,v in files.items()})
    if a.output.exists():raise ValueError('Refusing to overwrite existing analysis')
    a.output.write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':main()
