"""Derive pinned source references, mounting holes and component envelopes. No vendor binary copied."""
from pathlib import Path
import hashlib,json,xml.etree.ElementTree as E,math
import sys
P=Path(__file__).resolve().parents[1]
D=Path(sys.argv[1]).resolve() # unpack pinned vendor sources outside the repository
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
entries=[]
for name,url,role in [
('Click2-officiel.zip','https://download.mikroe.com/documents/2d-3d-files/RS485_Isolator_2_click-2d-3d-files.zip','Official archive; v101 STEP actual163solids, schematic v102 distinct'),
('ST-kit-gerber.zip','https://www.st.com/resource/en/board_manufacturing_specification/steval-mki208v1k_gerber.zip','Official ST Gerbers adapter+sensor'),
('FRAM-official.brd','https://raw.githubusercontent.com/adafruit/Adafruit-FRAM-Breakout-PCB/master/SPI%20FRAM%202%20and%204%20Mbit.brd','Eagle FRAM4718 exact drills, component footprint envelopes'),
('SD-official.brd','https://raw.githubusercontent.com/adafruit/Adafruit-MicroSD-SPI-or-SDIO-card-breakout-PCB/master/Adafruit%20SDIO%20microSD.brd','Eagle SD4682 exact drills'),
('Connfly-DS1023.pdf','https://www.connfly.com/static/upload/file/pdf/DS1023.pdf','Manufacturer DS1023 housing8.5, tail3.0±.2, length25.9±.3; NO contact depth provided'),
('Samtec-SHF.pdf','https://suddendocs.samtec.com/catalog_english/shf.pdf','Manufacturer SHF-SM height5.46 width5.85,6perrow length13.97; assembled cable envelope design allowance')]:
 f=D/name;entries.append({'file_local':name,'url':url,'bytes':f.stat().st_size,'sha256':sha(f),'role':role,'redistributed':False})
sp=next((D/'Click2').rglob('RS485_Isolator_2_click_v101.step'));entries.append({'archive_member':str(sp.relative_to(D/'Click2')),'sha256':sha(sp),'solids':163,'world_transform':'rotate Z -90 then translation(87.22,-80,B PCBtop)','redistributed':False})
for f in sorted((D/'ST-kit/adapter').rglob('*')):
 if f.is_file():entries.append({'archive_member':str(f.relative_to(D/'ST-kit')),'sha256':sha(f),'bytes':f.stat().st_size,'redistributed':False})
sd=P.parent/'mechanical/V1_5/models/Adafruit_4682.step'
entries.append({'source_existing_repository':'mechanical/V1_5/models/Adafruit_4682.step','sha256':sha(sd),'role':'Pinned official Adafruit4682 source reused byte-identically, native6solids; not added to printer ZIP','solids':6,'redistributed_again':False})
(P/'preuves/sources-fabricants.json').write_text(json.dumps(entries,indent=2,ensure_ascii=False)+'\n')
# Eagle source mounting holes + conservative component footprints; no vendor solids copied.
out={}
for name in ['FRAM','SD']:
 root=E.parse(D/(name+'-official.brd')).getroot();pk={(li.get('name'),pa.get('name')):pa for li in root.findall('.//libraries/library')for pa in li.findall('./packages/package')}
 rows=[];holes=[]
 for el in root.findall('.//elements/element'):
  pa=pk[el.get('library'),el.get('package')];rot=el.get('rot','R0');a=math.radians(float(rot.split('R')[-1]));ex=float(el.get('x'));ey=float(el.get('y'))
  def tr(x,y):
   if'M'in rot:x=-x
   X=ex+x*math.cos(a)-y*math.sin(a);Y=ey+x*math.sin(a)+y*math.cos(a)
   return(X+42.46,Y+56.46)if name=='FRAM'else(43.04-Y,108.54-X)
  if'MOUNTINGHOLE'in pa.get('name'):
   p=pa.find('pad');holes.append({'name':el.get('name'),'xy':tr(0,0),'drill':float(p.get('drill')),'copper_diameter':float(p.get('diameter')),'floating':not any(c.get('element')==el.get('name')for c in root.findall('.//signals/signal/contactref'))});continue
  pads=pa.findall('smd')
  if not pads or el.get('name','').startswith('FID'):continue
  pts=[]
  for q in pads:
   x,y=float(q.get('x')),float(q.get('y'));dx,dy=float(q.get('dx')),float(q.get('dy'));ar=math.radians(float(q.get('rot','R0').split('R')[-1]))
   for u in [-dx/2,dx/2]:
    for w in [-dy/2,dy/2]:pts.append(tr(x+u*math.cos(ar)-w*math.sin(ar),y+u*math.sin(ar)+w*math.cos(ar)))
  # Include package assembly/silkscreen outline to avoid reducing a chip to its pads.
  for q in pa.findall('wire'):
   if q.get('layer')in['21','51']:
    for n in [1,2]:pts.append(tr(float(q.get('x'+str(n))),float(q.get('y'+str(n)))))
  if not pts:continue
  xs,ys=zip(*pts);rows.append({'name':el.get('name'),'package':pa.get('name'),'bbox_XY':[min(xs)-.25,min(ys)-.25,max(xs)+.25,max(ys)+.25],'body_height_design_envelope_not_supplier':4.0})
 out[name]={'mount_holes':holes,'component_envelopes':rows,'method':'Eagle exact placements and rotations, union SMD+solder courtyard/silkscreen AABB expanded0.25; Zheight4mm conservative design assumption, not measured manufacturer height'}
(P/'sources/adafruit-derives.json').write_text(json.dumps(out,indent=2,ensure_ascii=False)+'\n')
print('saved',len(entries),'source entries and',[(k,len(v['component_envelopes']))for k,v in out.items()])
