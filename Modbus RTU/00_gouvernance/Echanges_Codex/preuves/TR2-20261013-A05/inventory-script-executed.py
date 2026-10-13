from pathlib import Path
import subprocess,json,re,hashlib,datetime
r=Path.cwd();p=Path('/home/lolo/dev/msm/reports')/('TR2-A05-'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'));p.mkdir();Path('/tmp/tr2-a05-proof').write_text(str(p));fw=r/'Modbus RTU/05_Firmware';cm=(fw/'CMakeLists.txt').read_text()
old='/home/lolo/dev/msm/projects/MSM_vibe-M015-V/Modbus RTU/05_Firmware/build-host-validation'
z=subprocess.run(['ctest','--test-dir',old,'--show-only=json-v1'],capture_output=True);z.check_returncode();inventory=json.loads(z.stdout);assert len(inventory['tests'])==125
(p/'ctest-discovery.json').write_bytes(z.stdout)
links={}
for match in re.finditer(r'tr2_add_(?:core|host)_test\((test_\w+)\s+(\S+)\s+(\w+)\)',cm):
 links[match[3]]={'target':match[1],'sources':[match[2]]}
executables={}
for m in re.finditer(r'add_executable\((test_[^\s)]+)(.*?)\)',cm,re.S):
 executables[m[1]]=m[2].split()
for m in re.finditer(r'add_test\(NAME\s+(\w+)\s+COMMAND\s+(test_\w+)\)',cm):
 links[m[1]]={'target':m[2],'sources':executables.get(m[2],[])}
for tx in (0,1):
 for obs in (0,1):
  name=('stm32_irq_observer_' if obs else 'stm32_rtu_tx_')+str(tx)
  links[name]={'target':'test_'+name,'sources':['tests/platform/test_stm32_rtu_tx.c','platform/stm32/stm32_serial_transport.c','platform/stm32/stm32_modbus_diagnostic.c'],'macros':{'TX_QUALIFIED':tx,'IRQ_DIAGNOSTICS':obs}}
rows=[]
for test in inventory['tests']:
 name=test['name'];assert name in links,name;row={'ctest_name':name,**links[name]}
 resolved=[]
 for item in row['sources']:
  f=Path(item.replace('${ST_REF}',str(r/'experiments/iis3dwb_st_reference')))
  if not f.is_absolute():f=fw/f
  assert f.exists(),f
  entry={'path':str(f.relative_to(r)),'sha256':hashlib.sha256(f.read_bytes()).hexdigest()}
  # A transport mock includes the actual adapter rather than listing it as a CMake source.
  entry['included_C_files']=re.findall(r'#include\s+"([^"]+\.c)"',f.read_text()) if f.suffix=='.c' else []
  resolved.append(entry)
 row['resolved_sources']=resolved;row['historical_execution']='M015-V full host log125/125; not reexecuted in A05';rows.append(row)
(p/'firmware-tests-inventory.json').write_text(json.dumps(rows,indent=2)+'\n')
others={'root_test_files':{},'standalone_ST_CTest_names':re.findall(r'add_test\(NAME\s+(\w+)',(r/'experiments/iis3dwb_st_reference/CMakeLists.txt').read_text()),'supervision_test_projects':[str(x.relative_to(r)) for x in sorted((r/'Modbus RTU/Supervision TR2/tests').glob('*/*.csproj'))],'additional_Python_tests':[str(x.relative_to(r)) for x in sorted(r.rglob('test_*.py')) if '.git' not in str(x) and 'build-' not in str(x)]}
for ext in ('.c','.h','.py'):others['root_test_files'][ext]=len(list((fw/'tests').rglob('*'+ext)))
(p/'other-tests-inventory.json').write_text(json.dumps(others,indent=2)+'\n')
proofstatus={}
for folder in ['TR2-20261013-M015-V','TR2-20261013-A04']:
 root=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves'/folder;count=0
 for line in (root/'MANIFEST.sha256').read_text().splitlines():
  h,n=line.split('  ',1);assert hashlib.sha256((root/n).read_bytes()).hexdigest()==h,n;count+=1
 proofstatus[folder]={'manifest_sha256':hashlib.sha256((root/'MANIFEST.sha256').read_bytes()).hexdigest(),'file_count_verified':count}
log=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261013-M015-V/validation-after-refresh-fix.log';assert '100% tests passed, 0 tests failed out of 125' in log.read_text()
proofstatus['host_log']={'path':str(log.relative_to(r)),'sha256':hashlib.sha256(log.read_bytes()).hexdigest(),'verified_result':'125/125 historical','new_execution':False}
(p/'historical-proofs-verified.json').write_text(json.dumps(proofstatus,indent=2)+'\n')
protected={}
for n in ['AGENTS.md','Modbus RTU/00_gouvernance/DETTE_TECHNIQUE_A04.md','Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md','Modbus RTU/00_gouvernance/ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md']:
 protected[n]=hashlib.sha256((r/n).read_bytes()).hexdigest()
(p/'preservation-baseline.json').write_text(json.dumps({'initial_HEAD':subprocess.check_output(['git','rev-parse','HEAD']).decode().strip(),'protected_sha256':protected,'stashes':subprocess.check_output(['git','rev-parse','stash@{0}','stash@{1}']).decode().splitlines()},indent=2)+'\n')
(p/'inventory-script-executed.py').write_bytes(Path(__file__).read_bytes())
print('125 CTest entries mapped to real sources; historical manifests validated. Other inventory:',json.dumps(others['root_test_files']),len(others['supervision_test_projects']),'supervision test projects. Evidence:',p)
