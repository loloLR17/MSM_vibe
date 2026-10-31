from pathlib import Path
import sys,struct,json,re,subprocess
P=Path('/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-N8B-20261031');sys.path.insert(0,str(P));import hardware_second as h
ROOT=Path('/home/lolo/dev/msm/projects/MSM_vibe-R3-8h-faststart');MAIN=ROOT/'experiments/r3_durable_fat32/src/main.c'
import ctypes
s=MAIN.read_text();body=s[s.index(' uint32_t version,stage'):s.index('} Result;')];layout=[]

body=re.sub(r'#if R3_ENDURANCE_8H\n(.*?)#else\n.*?#endif',r'\1',body,flags=re.S)
for kind,group in re.findall(r'(uint32_t|uint64_t)\s+([^;]+);',body):
 for part in group.split(','):
  m=re.fullmatch(r'(\w+)(?:\[(\d+)\])?',part.strip());assert m
  typ=ctypes.c_uint32 if kind=='uint32_t' else ctypes.c_uint64
  layout.append((m[1],typ*int(m[2]) if m[2] else typ))
class Result(ctypes.LittleEndianStructure):_fields_=layout
def analyze(d):
 raw=(d/'result.bin').read_bytes();assert len(raw)==ctypes.sizeof(Result),(len(raw),ctypes.sizeof(Result));obj=Result.from_buffer_copy(raw);r={}
 for name,typ in layout:
  value=getattr(obj,name)
  if isinstance(value,ctypes.Array):r.update({name+'_'+str(i):v for i,v in enumerate(value)})
  else:r[name]=value
 assert [r['uid_'+str(i)] for i in range(3)]==[0x001b0046,0x38425018,0x20333157] and r['version']==4 and r['clock']==160000000
 (d/'result.json').write_text(json.dumps(r,indent=2)+'\n');return r
def run(name,previous_run,cmd,timeout=45,offset=None):
 d=P/name;d.mkdir(exist_ok=False)
 b=h.IDENTITY+f'''p r3_result
if r3_result.error || r3_result.run != {previous_run} || r3_command || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 & 0x1000)
 quit 4
end
'''+('if r3_result.stage != 2\n quit 5\nend\n' if previous_run==0 else 'if r3_result.stage != 4\n quit 5\nend\n')
 if offset is not None:b+=f'set var r3_return_offset={offset}\n'
 b+=f'thbreak r3_finished\nset var r3_command=0x{0x44335200|cmd:08x}\ncontinue\np r3_result\np r3_writer\np r3_card\np durable_status\np r3_spi_fault\np q.count\np sizeof(r3_distributions[0])\np sizeof(r3_sd_distributions[0])\n'
 dumps=[('busy-distributions','durable_busy_distributions','(durable_busy_distributions+2)'),('single-busy-max','&durable_single_busy_max','(&durable_single_busy_max+1)'),('durable-status','&durable_status','(&durable_status+1)'),('durable-footer','durable_footer','(durable_footer+512)'),('spi3-config','0x46002008','0x46002010'),('spi2-config','0x40003808','0x40003810'),('result','&r3_result','(&r3_result+1)'),('card','&r3_card','(&r3_card+1)'),('records','r3_records','(r3_records+r3_result.trace_count)'),('distributions','r3_distributions','(r3_distributions+4)'),('sd-records','r3_sd_records','(r3_sd_records+(r3_result.sd_completed<2048?r3_result.sd_completed:2048))'),('sd-distributions','r3_sd_distributions','(r3_sd_distributions+2)'),('brackets','r3_brackets','(r3_brackets+r3_result.cal_brackets)'),('raw','r3_raw','(r3_raw+r3_result.raw_words*7)'),('sources','r3_sources','(r3_sources+r3_source_count)'),('outputs','r3_outputs','(r3_outputs+r3_output_count)'),('source-sha','r3_source_sha','(r3_source_sha+32)'),('header','r3_header','(r3_header+512)'),('fifo-stop-trace','bench_trace','(bench_trace+bench_trace_count)'),('fifo-stop-payload','bench_payload','(bench_payload+bench_payload_count)')]
 if cmd==0x10:dumps += [('return','r3_return','(r3_return+r3_result.return_bytes)'),('media-header','r3_media_header','(r3_media_header+512)')]
 for n,start,end in dumps:
  f=d/(n+'.bin');f.write_bytes(b'');path=h.win(f).replace('\\','/');assert not any(c.isspace() or c=='"' for c in path)
  b+=f'if {end} > {start}\ndump binary memory {path} {start} {end}\nend\n'
 b+='if r3_result.stage != 4 || r3_result.error || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 & 0x1000)\n quit 6\nend\n'
 (d/'body.gdb').write_text(b)
 try:h.debug(name,P/'arm/r3_coex.elf',b,timeout=timeout)
 except subprocess.CalledProcessError:
  if (d/'result.bin').stat().st_size:print('CAPTURED FAILURE',json.dumps(analyze(d)),flush=True)
  raise
 r=analyze(d);assert r['stage']==4 and not r['error'];print(json.dumps(r),flush=True);return r
if __name__=='__main__':run(sys.argv[1],int(sys.argv[2]),int(sys.argv[3],0))
