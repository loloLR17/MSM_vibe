"""R0 documentary checks only: no firmware import, build, debugger or hardware I/O."""
from pathlib import Path
import csv,json,hashlib,struct,zlib,re,subprocess
root=Path.cwd(); norm=root/'Modbus RTU/01_Specification_source/LIVE_R0'; val=root/'Modbus RTU/02_Validation/LIVE_R0'; proof=root/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261015-R0'
def readcsv(p):return list(csv.DictReader(p.open()))
def save(name,obj):(proof/name).write_text(json.dumps(obj,ensure_ascii=False,indent=2)+'\n')
def bitcrc(data):
 c=0xffffffff
 for b in data:
  c^=b
  for _ in range(8):c=(c>>1)^0xedb88320 if c&1 else c>>1
 return c^0xffffffff
res=json.loads((norm/'reservations.json').read_text());assert res['protocol_version']==2 and res['media_logical_version']==3 and res['media_physical_version']==1
assert len(set(res['commands'].values()))==3 and set(res['commands'].values())=={12,13,14}
assert res['b0_live_bit']==4 and res['tuple_live']==dict(acquisition_mode=2,storage_mode=0,storage_limit_mb=0,campaign_duration_s=0)
rows=readcsv(val/'mapping_live.csv');assert len(rows)==187
old=root/'Modbus RTU/02_Validation/mapping_unifie'; inherited=[x for x in readcsv(old/'tr2_mapping_unifie_logique.csv') if x['block']!='5']+readcsv(old/'V1_1/tr2_mapping_unifie_logique_v1_1.csv')
expected={(x['block'],x['logical_name']):x for x in inherited}
seen=set();counts={};extents={}
for x in rows:
 key=x['block'],x['logical_name']; y=expected[key]
 for field in ['address_start','address_end','offset_start','offset_end','declared_type','register_count','access','source_fields']:assert x[field]==y[field],(key,field)
 a,b=int(x['address_start']),int(x['address_end']);assert b-a+1==int(x['register_count'])
 for addr in range(a,b+1):assert addr not in seen;seen.add(addr)
 counts[x['block']]=counts.get(x['block'],0)+b-a+1
for i,n in enumerate([21,20,16,48,176,29,64,16]):
 base=1000*i;assert set(range(base,base+n))<=seen;assert counts[str(i)]==n;extents[str(i)]=n
assert len(seen)==390
vectors=json.loads((norm/'vectors.json').read_text());expected_crc={'V1_REFERENCE':('5207CCFC','092C7610'),'LIVE_A':('7BA855C8','7818AAB3'),'LIVE_B':('FC4001B4','E5F05014')}
crc_checks=[]
for x in vectors:
 for kind,n in [('prepared',84),('active',76)]:
  data=struct.pack('>'+str(n)+'H',*x[kind+'_registers']);assert len(data)==2*n
  c=bitcrc(data);assert c==zlib.crc32(data);assert f'{c:08X}'==x[kind+'_crc'];assert hashlib.sha256(data).hexdigest()==x[kind+'_sha256']
 assert (x['prepared_crc'],x['active_crc'])==expected_crc[x['id']];crc_checks.append(dict(id=x['id'],prepared=x['prepared_crc'],active=x['active_crc']))
frozen=(root/'Modbus RTU/01_Specification_source/bloc4.md').read_text();pairs={int(a):int(b,16) for a,b in re.findall(r'`(40\d\d) = (0x[0-9A-Fa-f]+)`',frozen)}
assert [pairs.get(a,0) for a in range(4016,4100)]==vectors[0]['prepared_registers']
# Confirm source constants before the arithmetic rather than assuming a physical layout.
def constant(path,name):
 t=(root/path).read_text();m=re.search(r'^#define '+name+r'\s+(?:\(\(size_t\))?(\d+)u?',t,re.M);assert m,(path,name);return int(m[1])
inc='Modbus RTU/05_Firmware/include/tr2/persistence/'
assert constant(inc+'configuration_record.h','TR2_CONFIGURATION_RECORD_SIZE')==140
assert constant(inc+'campaign_record.h','TR2_CAMPAIGN_RECORD_SIZE')==252
assert constant(inc+'command_journal_bounded_record.h','TR2_COMMAND_JOURNAL_BOUNDED_RECORD_SIZE')==70
logical=[('configuration',0,280),('time',280,298),('campaign_repository',298,4370),('campaign_data',4370,15922),('journal',15922,51762),('diagnostic',51762,51802),('boot',51802,51818),('identity',51818,51914)]
assert 298+2*20+8*2*252==4370 and 4370+4*(2*36+32*88)==15922 and 15922+256*2*70==51762
assert all(logical[i][2]==logical[i+1][1] for i in range(len(logical)-1))
geo_text=(root/'Modbus RTU/05_Firmware/platform/stm32/stm32_production_board.c').read_text();assert '{262144,0,4096,8192,65536,57344}' in geo_text
old_regions=[(0,122880)];new_regions=[(122880,126976),(126976,131072),(131072,188416),(188416,245760)];assert all(0<=a<b<=262144 for a,b in new_regions)
assert all(b<=c or d<=a for i,(a,b) in enumerate(new_regions) for c,d in new_regions[i+1:])
assert all(b<=c or d<=a for a,b in old_regions for c,d in new_regions)
assert 52298+64<=57344 and 2*128*112<=35840 and 51914+6*64==52298
# Every prefix of each planned write stays inside LIVE: documentary cut model, not a storage test.
writes=[('payload',131072+64,52298),('header',131072,64),('superblock',122880,64)]
cut_positions=1
for name,a,n in writes:
 for k in range(n+1):
  assert a>=122880 and a+k<=245760
  assert not any(a<d and a+k>c for c,d in old_regions)
 cut_positions+=n
layout=dict(source_layout=[dict(name=n,start=a,end=b) for n,a,b in logical],historical_protected=[0,122880],live_regions=new_regions,logical_live_size=52298,bank_slack=57344-64-52298,journal_size=128*2*112,model_cut_positions=cut_positions,limit='Modele des intervalles planifies uniquement; aucun test de migration du firmware, aucune preuve de disponibilite physique du tail ni de resistance a une coupure reelle.')
save('layout-review.json',layout)
# Explicit cross-version and continuous-duration normative oracles, no C implementation executed.
decisions=readcsv(val/'decisions_start_stop.csv');assert len(decisions)==384
assert len({tuple(x[k] for k in ['command','configuration','acquisition','blocking_fault','maintenance','journal_budget','nonterminal_transaction']) for x in decisions})==384
assert set(x['expected'] for x in decisions)=={'5/13','8/18','5/8','5/22','5/3','5/7','5/21','ADMIT_PENDING_EFFECT'}
tests=readcsv(val/'tests_live.csv');assert len(tests)==106;assert len({t['id'] for t in tests})==106;assert all(t['status']=='SPECIFIED_NOT_EXECUTED' for t in tests)
reqs=set(re.findall(r'\*\*(R0-[A-Z]+)\.?\*\*',(norm/'PROFIL_LIVE.md').read_text()+(norm/'MIGRATION_FRAM.md').read_text()));covered=set(re.findall(r'R0-[A-Z]+',' '.join(t['requirement'] for t in tests)));assert reqs<=covered,reqs-covered
for t in tests:
 if t['historical_reference']:assert (root/'Modbus RTU/02_Validation'/t['historical_reference']).is_file()
initial=json.loads((proof/'initial-state.json').read_text())
for x in initial['protected']:assert hashlib.sha256((root/x['path']).read_bytes()).hexdigest()==x['sha256'],x['path']
for f in list(norm.glob('*.md'))+list(val.glob('*.md')):
 for link in re.findall(r'\]\(([^)]+)\)',f.read_text()):assert (f.parent/link.split('#')[0]).exists(),(f,link)
result=dict(status='DOCUMENTARY_CHECKS_PASSED',mapping_fields=len(rows),decision_contexts=len(decisions),mapped_registers=len(seen),block_lengths=extents,numeric_reservations_checked=True,crc_vectors=crc_checks,crc_implementations='bitwise independant + zlib; fixture historique relue depuis bloc4.md',test_oracles_specified=len(tests),requirement_ids=sorted(reqs),historical_files_unchanged=len(initial['protected']),layout_proof='layout-review.json',limits='Ces controles ne sont ni une implementation LIVE ni une execution des 106 tests firmware/PC/physiques. Revue independante ChatGPT requise avant R1.')
save('document-checks.json',result);print(json.dumps(result,ensure_ascii=False))
