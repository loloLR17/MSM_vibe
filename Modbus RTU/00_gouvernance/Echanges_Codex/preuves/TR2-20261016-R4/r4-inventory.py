from pathlib import Path
import subprocess as s,json,re
root=Path.cwd();out=Path('/home/lolo/dev/msm/reports/TR2-r4');fw=root/'Modbus RTU/05_Firmware'
old=json.loads((root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261015-R3A/r3a-module-inventory.json').read_text());by={q['path']:q for q in old['modules']}
cmakes=[q for q in root.rglob('CMakeLists.txt') if not any(part.startswith('build') for part in q.parts)]
links={}
for line in (out/'resources/canonical-symbols.txt').read_text().splitlines():
 if '\t'+str(root)+'/' in line:
  sym,source=line.split('\t',1);name=source.rsplit(':',1)[0];name=name.removeprefix(str(root)+'/');links.setdefault(name,[]).append(sym.split()[-1])
for name in s.check_output(['git','ls-files','-z']).decode().split('\0'):
 if not name or not name.endswith(('.c','.h')) or not (name.startswith('Modbus RTU/05_Firmware/') or name.startswith('experiments/')) or any(q.lower() in ['vendor','generated','drivers','third_party'] for q in Path(name).parts):continue
 q=by.setdefault(name,{'path':name});base=Path(name).name
 q['cmake_filename_references']=[str(p.relative_to(root)) for p in cmakes if base in p.read_text()]
 q['production_symbols']=links.get(name,[])
 if q['production_symbols']:q['classification']='linked_canonical_elf'
 elif '/tests/' in name:q['classification']='host_test_only'
 elif name.endswith('.h'):q['classification']='required_interface_not_link_unit'
 elif '/src/' in name and 'Modbus RTU/05_Firmware/' in name:
  q['classification']='prepared_or_library_not_default_linked';q['arm_compile_evidence']='platform/stm32/CMakeLists.txt GLOB_RECURSE portable src/*.c; archive built in final-arm.log'
 elif 'live_fifo_acquisition' in name or 'stm32_live_acquisition' in name:q['classification']='prepared_R3_not_default_linked'
 else:q['classification']='historical_or_specialized_or_tested_library_not_default_linked'
result={'method':'refresh R3B inventory with git ls-files, canonical nm -l paths, literal CMake references; portable src glob documented; nonlinked is NOT dead','code_sha':s.check_output(['git','rev-parse','HEAD']).decode().strip(),'modules':list(sorted(by.values(),key=lambda q:q['path']))}
(out/'module-inventory.json').write_text(json.dumps(result,indent=2)+'\n');print(len(by),'entries',len(links),'linked source paths')
