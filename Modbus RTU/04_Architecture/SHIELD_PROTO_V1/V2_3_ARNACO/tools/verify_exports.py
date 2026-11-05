"""Independently compare exported netlist, saved PCB readback and CAM drill/pad data.
Run after export_design.py and read_board.py. No firmware/hardware validation.
"""
from pathlib import Path
from collections import Counter
import json,re,xml.etree.ElementTree as ET,math,hashlib
P=Path(__file__).resolve().parents[1];d=json.loads((P/'design.json').read_text());b=json.loads((P/'docs/checks/pcb-readback.json').read_text());m=P/'exports/CAM_CANDIDAT_NON_LIBERE'
expected={(c['ref'],n):net for c in d['components']for n,net in c['nets'].items()}
root=ET.parse(P/'exports/netlist.xml').getroot();schematic={(v.attrib['ref'],v.attrib['pin']):n.attrib['name']for n in root.findall('./nets/net')for v in n.findall('node')if not n.attrib['name'].startswith('unconnected-')}
assert schematic==expected,('netlist mismatch',schematic.items()^expected.items())
actual={(q['ref'],q['number']):q['net']for q in b['pads']if q['net']and not q['net'].startswith('unconnected-')};assert actual==expected,'PCB connectivity differs from schematic'
all_sch={(v.attrib['ref'],v.attrib['pin']):n.attrib['name']for n in root.findall('./nets/net')for v in n.findall('node')}
all_pcb={(q['ref'],q['number']):q['net']for q in b['pads']if q['number']}
assert all_pcb==all_sch,'PCB NC islands differ from exported schematic'
assert {c.attrib['ref']for c in root.findall('./components/comp')}=={c['ref']for c in d['components']}
# X2 attributes are read from actual CAM output, not synthesized from the matrix.
for layer in ['F_Cu.gtl','B_Cu.gbl']:
 text=(m/('TR2_Shield_Proto_V1-'+layer)).read_text();pad=None;net=None;cam={}
 for line in text.splitlines():
  if line=='%TD*%':pad=None;net=None
  if line.startswith('%TO.P,'):pad=tuple(line[6:-2].split(',')[:2])
  if line.startswith('%TO.N,'):net=line[6:-2]
  if re.match(r'X-?\d+Y-?\d+D03\*',line)and pad and net and net!='N/C'and not net.startswith('unconnected-'):cam[pad]=net
 assert cam==expected,('CAM attributes mismatch',layer,cam.items()^expected.items())
# Compare every drill center and diameter, including NPTH and vias.
# KiCad metric Excellon outputs three decimal places (1um). Compare at actual CAM resolution.
# Native-to-ST XY matching remains independently checked at2nm; do not round the PCB itself.
def norm(x,y,drill):return round(x,3),round(y,3),round(drill,3)
for kind,npth in [('NPTH',True),('PTH',False)]:
 want=Counter(norm(q['x'],q['y'],q['drill'])for q in b['pads']if q['npth']==npth)
 if not npth:want.update(norm(t['x'],t['y'],.3)for t in b['tracks']if t['via'])
 tools={};got=Counter();tool=None
 for l in (m/f'TR2_Shield_Proto_V1-{kind}.drl').read_text().splitlines():
  mt=re.fullmatch(r'T(\d+)C([\d.]+)',l)
  if mt:tools[mt[1]]=float(mt[2])
  elif re.fullmatch(r'T\d+',l):tool=l[1:]
  else:
   mt=re.fullmatch(r'X(-?[\d.]+)Y(-?[\d.]+)',l)
   if mt:got[norm(float(mt[1]),-float(mt[2]),tools[tool])]+=1
 assert got==want,('drill mismatch',kind,got-want,want-got)
outline=(m/'TR2_Shield_Proto_V1-Edge_Cuts.gm1').read_text();coords=[(int(x)/1e6,-int(y)/1e6)for x,y in re.findall(r'X(-?\d+)Y(-?\d+)D0[12]',outline)]
assert set(coords)=={tuple(p)for p in d['board_outline_mm']},'CAM board outline differs'
lengths={}
for t in b['tracks']:
 if not t['via']:lengths[t['net']]=lengths.get(t['net'],0)+t['length_mm']
result={'status':'PASS','scope':'connectivity + both copper X2 pad attributes + all Excellon holes + rectangular CAM outline; no independent CAM copper topology/impedance or physical fit qualification','connected_pins':len(expected),'footprints':len(d['components']),'pads':len(b['pads']),'vias':sum(t['via']for t in b['tracks']),'outline_mm':[90,100],'track_lengths_per_net_mm':lengths,'manufacturing_release':'BLOCKED'}
(P/'docs/checks/export-verification.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
