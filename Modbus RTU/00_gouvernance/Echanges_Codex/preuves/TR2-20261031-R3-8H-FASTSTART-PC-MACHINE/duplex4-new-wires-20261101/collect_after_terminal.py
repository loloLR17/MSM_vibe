from pathlib import Path
import ast,sys,json
import hardware_second as h
import run_capture as c
import analyze_capture as a
P=Path(__file__).parent

def collect(name,expected_run,witness_name):
 w=json.loads((P/(witness_name+'-analysis.json')).read_text());assert w['terminal']['flags']==(4|(expected_run<<8))
 tree=ast.parse((P/'run_capture.py').read_text());dumps=None
 for n in ast.walk(tree):
  if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='dumps' for t in n.targets):dumps=ast.literal_eval(n.value);break
 assert dumps;d=P/name;d.mkdir(exist_ok=False)
 body=h.IDENTITY+f'''p r3_result
p r3_writer
p witness_sequence
p witness_errors
if r3_result.stage!=4 || r3_result.error || r3_result.run!={expected_run} || r3_command || fifo.running || r3_writer.active || q.count || witness_errors || r3_clock_fault || !(*(unsigned int*)0x42021414 &0x1000)
 quit 7
end
'''
 for title,start,end in dumps:
  f=d/(title+'.bin');f.write_bytes(b'');path=h.win(f).replace('\\','/');body+=f'if {end} > {start}\ndump binary memory {path} {start} {end}\nend\n'
 h.debug(name+'-collect-after-terminal',P/'arm/r3_coex.elf',body)
 r=c.analyze(d);assert r['accepted']>=w['terminal']['accepted'] and r['outputs']==(r['accepted']+9)//10
 result=a.analyze(name);assert result['pass_references'] and result['numeric_prefix_verified_outputs']==103
 print(json.dumps(result,indent=2),flush=True);return result
if __name__=='__main__':collect(sys.argv[1],int(sys.argv[2]),sys.argv[3])
