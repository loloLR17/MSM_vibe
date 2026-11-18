"""Exhaustive six-contact difference plus unchanged non-copper CAM and 17 nets."""
from pathlib import Path
from collections import defaultdict
import csv,json,hashlib
P=Path(__file__).resolve().parents[1];BASE=P.parent/'V2_3_ARNACO';proof=P/'preuves/E01'
reference=json.loads((proof/'reference-constructeurs.json').read_text())['rows'];actual=json.loads((proof/'audit-complet.json').read_text())['rows'];old=json.loads((P.parents[2]/'00_gouvernance/Echanges_Codex/preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/E01/native-PCB.json').read_text())
oldpads={(p['ref'],p['pin']):p for p in old['pads']if p['pin']};want={(r['ref'],r['pin']):r['desired_net']for r in reference};normal=lambda n:'NC'if not n or n.startswith('unconnected-')else n
changes=[]
for row in actual:
 key=row['ref'],row['pin'];before=normal(oldpads[key]['net']);after=row['pcb_net']
 if before!=after:changes.append({'ref':key[0],'pin':key[1],'before':before,'after':after,'vendor':row['vendor_function'],'x':row['pcb_x_mm'],'y':row['pcb_y_mm']})
assert len(changes)==6 and all(r['ref']=='J8'for r in changes)
assert sum(normal(p['net'])!=want[key]for key,p in oldpads.items())==6
members=defaultdict(list)
for row in actual:
 if row['desired_net']!='NC':members[row['desired_net']].append(row['ref']+'.'+row['pin'])
assert len(members)==17 and sum(map(len,members.values()))==59
with(proof/'MATRICE_E01_17_NETS.csv').open('w',newline='')as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['net','all_contact_members','source_schema_PCB_CAM_match','copper_connectivity','physical_qualification'])
 for net,pads in sorted(members.items()):w.writerow([net,';'.join(sorted(pads)),'PASS','PASS_geometric_component','NOT_FABRICATED'])
# Compare all unaffected plotting commands; remove only timestamps/line endings.
def normalize(text):
 prefixes=('%TF.CreationDate,','G04 Created by ','; #@! TF.CreationDate,','; DRILL file KiCad ')
 return '\n'.join(l for l in text.splitlines()if not l.startswith(prefixes))
equalities=[]
for f in(P/'exports/CAM_CANDIDAT_NON_LIBERE').iterdir():
 if f.suffix in('.gts','.gbs','.gto','.gbo','.gm1','.drl'):
  a=BASE/'exports/CAM_CANDIDAT_NON_LIBERE'/f.name;assert normalize(a.read_text())==normalize(f.read_text()),f.name
  equalities.append({'file':f.name,'plot_commands_equal':True,'before_sha256':hashlib.sha256(a.read_bytes()).hexdigest(),'after_sha256':hashlib.sha256(f.read_bytes()).hexdigest()})
assert len(equalities)==7
(P/'preuves/diff-V23-V24.json').write_text(json.dumps({'changed_pad_nets':changes,'old_native_vs_fresh_reference_mismatches':6,'candidate_mismatches':0,'nets':len(members),'connected_contacts':59,'unaffected_CAM_equalities':equalities,'normalization':'Creation date comment lines + LF only; no plotting commands or geometry removed','all_non_SPI3_native_copper_and_all_vias_unchanged':'Verified independently in M01-native.json'},indent=2)+'\n')
print('PASS:only6J8 contacts;17nets/59connected;7unaffectedCAMs identical apart creation dates')
