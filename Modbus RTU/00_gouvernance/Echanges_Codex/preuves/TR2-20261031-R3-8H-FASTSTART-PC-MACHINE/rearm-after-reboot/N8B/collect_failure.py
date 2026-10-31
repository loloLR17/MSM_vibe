from pathlib import Path
import ast,json
import hardware_second as h
import run_capture as c
P=Path(__file__).parent;d=P/'n8b-failed-gate';d.mkdir(exist_ok=False)
tree=ast.parse((P/'run_capture.py').read_text());dumps=next(ast.literal_eval(n.value) for n in ast.walk(tree) if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='dumps' for t in n.targets))
body=h.IDENTITY+'''p r3_result
p r3_writer
p fifo
p durable_status
p r3_clock_fault
if fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
 quit 8
end
'''
for title,start,end in dumps:
 f=d/(title+'.bin');f.write_bytes(b'');body+=f'if {end} > {start}\ndump binary memory {h.win(f).replace(chr(92),"/")} {start} {end}\nend\n'
h.debug('n8b-failure-state',P/'arm/r3_coex.elf',body);print(json.dumps(c.analyze(d),indent=2))
