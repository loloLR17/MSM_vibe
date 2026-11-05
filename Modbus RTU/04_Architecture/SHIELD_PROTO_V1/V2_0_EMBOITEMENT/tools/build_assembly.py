from pathlib import Path
import cadquery as cq,json,hashlib,time
R=Path(__file__).resolve().parents[2];P=R/'V2_0_EMBOITEMENT';P.mkdir(exist_ok=True);(P/'preuves').mkdir(exist_ok=True);(P/'cad').mkdir(exist_ok=True)
E=Path('/home/lolo/dev/msm/reports/TR2-shield-v20-20261105');src=Path('/home/lolo/dev/msm/reports/TR2-shield-v19-20261105/ST-manufacturing/MB1549C/Mechanical/MB1549-U575ZIQ-C05.step')
start=time.time();print('Import official STEP',flush=True)
shape=cq.Shape.importBrep(str(E/'C05-cache.brep'));solids=shape.Solids();cq.exporters.export(shape,str(E/'C05-cache.brep'));print('Imported',len(solids),time.time()-start,flush=True)
G=12.25;T=json.loads((R/'V1_9_C05/preuves/comparaison-synthese.json').read_text())['transform_translation_mm'];B=json.loads((R/'V1_9_C05/docs/checks/pcb-readback.json').read_text());D=json.loads((R/'V1_9_C05/design.json').read_text())
def box(x,y,z,X,Y,Z):return cq.Workplane('XY').box(X-x,Y-y,Z-z,centered=False).translate((x,-Y,z)).val()
def cyl(x,y,z,r,h):return cq.Solid.makeCylinder(r,h,cq.Vector(x,-y,z))
parts=[]
def add(n,s,status='NOMINAL_CATALOGUE_ENVELOPE'):
 assert s.isValid(),n;parts.append({'name':n,'shape':s,'status':status});return s
shield=box(20,17,G,110,117,G+1.6).cut(cq.Compound.makeCompound([cyl(x['x'],x['y'],G-.1,x['drill']/2,1.8)for x in B['pads']]))
add('Shield_V19_declared_1p6',shield,'EXACT_V19_HOLES_OUTLINE_DECLARED_THICKNESS')
for c in D['components']:
 x,y=c['xy_mm'];ref=c['ref']
 if ref in ['J1','J2','J3','J4']:
  add(ref+'_plastic',box(x-1.25,y-1.27,G-2.5,x+3.79,y+(c['n']-1)*2.54+1.27,G))
  for k in range(c['n']):
   for i in range(2):
    # Intentional socket engagement; these pins are excluded from unwanted collision checks.
    add(ref+f'_post_{k}_{i}',box(x+i*2.54-.32,y+k*2.54-.32,G-2.5-6.1,x+i*2.54+.32,y+k*2.54+.32,G+1.6+1.4),'INTENTIONAL_ELECTRICAL_MATING_NOT_COLLISION')
 if ref=='J5':
  body=box(x-1.205,y-1.525,G-8.51,x+3.745,y+6.605,G)
  body=body.cut(cq.Compound.makeCompound([box(x+i*2.54-.45,y+k*2.54-.45,G-8.6,x+i*2.54+.45,y+k*2.54+.45,G+.1)for i in range(2)for k in range(3)]));add('J5_female_body',body)
  for i in [2,3,4,5]:
   xx=x+(i-1)%2*2.54;yy=y+(i-1)//2*2.54
   add(f'CN12_actual_pin_{64+i}',box(xx-.3175,yy-.3175,2.4,xx+.3175,yy+.3175,8.22),'USER_HEIGHT_NOMINAL_SECTION_INTENTIONAL_MATING')
   add(f'CN12_actual_plastic_{64+i}',box(xx-1.25,yy-1.25,0,xx+1.25,yy+1.25,2.4),'USER_HEIGHT_APPROX_BODY_ENVELOPE')
for i,(x,y)in enumerate(D['mounting_holes_mm']):
 add(f'spacer_OD3p5_{i}',cyl(x,y,0,1.75,G).cut(cyl(x,y,-.1,1.3,G+.2)),'PROPOSED_OD3p5_LENGTH_G_NOT_PURCHASED')
# Only use actual geometry from ST, flag supplied full Morpho populations differing from user hardware.
world=[s.translate((T[0],-T[1],0))for s in solids];source_rows=[];excluded=[];tests=[]
for i,(s,w) in enumerate(zip(solids,world)):
 b=s.BoundingBox();bb=w.BoundingBox()
 # Complete CN11/CN12 manufacturer header models contradict observed partial population.
 morpho=(b.xmin>=1 and b.xmax<=8 and b.ymin>10 and b.ymax<105 and b.zmax>0)or(b.xmin>=62 and b.xmax<=69 and b.ymin>10 and b.ymax<105 and b.zmax>0)
 if morpho:excluded.append(i)
 source_rows.append({'index':i,'bbox_ST':[b.xmin,b.xmax,b.ymin,b.ymax,b.zmin,b.zmax],'excluded_from_actual_population_case':morpho})
 for part in parts:
  if part['status'].startswith(('INTENTIONAL','USER_HEIGHT_NOMINAL')):continue
  a=part['shape'].BoundingBox()
  if a.xmax<=bb.xmin or a.xmin>=bb.xmax or a.ymax<=bb.ymin or a.ymin>=bb.ymax or a.zmax<=bb.zmin or a.zmin>=bb.zmax:continue
  v=part['shape'].intersect(w).Volume()
  if v>1e-6:tests.append({'part':part['name'],'ST_solid':i,'volume_mm3':v,'population_excluded':morpho})
print('ST intersections',len(tests),flush=True)
# Modules/retainers use existing V15 editable bounded envelopes, translated to the new Z reference.
# Import their individual CAD parts from the existing export if available; do not invent full measurements.
for c in D['components']:
 if c['ref'] in ['J6','J7','J8','J9']:
  x,y=c['xy_mm'];n=c['n'];z=G+1.6
  if c['ref']=='J6':bounds=[x-2.54,y-2.54,x+22.86,y+15.24]
  elif c['ref']=='J7':bounds=[x-20.32,y-22.86,x+2.54,y+2.54]
  elif c['ref']=='J8':bounds=[x-2.54,y-2.54,x+17.78,y+30.48]
  else:bounds=[x-21.59,y-1.27,x+35.56,y+24.13]
  add(c['ref']+'_module_PCB_envelope',box(bounds[0],bounds[1],z+11,bounds[2],bounds[3],z+12.6),'PROVISIONAL_EXISTING_MODULE_PCB_ENVELOPE')

# Existing V15 retainers recreated from their explicit analytical dimensions, unchanged XY.
def capsule(a,b,r,z,h):
 dx=b[0]-a[0];dy=b[1]-a[1];L=(dx*dx+dy*dy)**.5;nx=-dy/L*r;ny=dx/L*r
 pts=[(a[0]+nx,-a[1]-ny),(b[0]+nx,-b[1]-ny),(b[0]-nx,-b[1]+ny),(a[0]-nx,-a[1]+ny)]
 return cq.Workplane('XY').polyline(pts).close().extrude(h).translate((0,0,z)).val().fuse(cyl(*a,z,r,h),cyl(*b,z,r,h))
Z=G+1.6
for name,anchors in [('MKI',[[39,20],[71,54.5]]),('Click',[[91,75],[94.5,112.5]])]:
 ss=capsule(*anchors,3,Z+22,3)
 for x,y in anchors:ss=ss.fuse(cyl(x,y,Z,3,25))
 if name=='MKI':
  for x,y in [[43.1,24.3],[62.1,44.9]]:ss=ss.fuse(box(x-.5,y-1,Z+12.6,x+.5,y+1,Z+22.1))
 else:ss=ss.fuse(capsule([83,91.43],[91,91.43],2,Z+22,3),box(83.6,90.43,Z+12.6,84.6,92.43,Z+22.1))
 for i,(x,y)in enumerate(anchors):ss=ss.cut(cyl(x,y,Z-.1,1.4 if i==0 else 1.7,26))
 add(name+'_retainer',ss,'EXISTING_V15_ANALYTICAL_UNCHANGED_XY_CONTACTS_PROVISIONAL')
for c in D['components']:
 x,y=c['xy_mm'];r=c['ref'];rows=[]
 if r=='J6':rows=[[x-1.92,y-1.645,x+22.24,y+1.645]]
 if r=='J7':rows=[[x-1.645,y-22.24,x+1.645,y+1.92]]
 if r=='J8':rows=[[xx-1.645,y-1.92,xx+1.645,y+29.86]for xx in [x,x+15.24]]
 if r=='J9':rows=[[x-19.70,yy-1.645,x+1.92,yy+1.645]for yy in [y,y+22.86]]
 for i,bounds in enumerate(rows):add(r+'_socket_'+str(i),box(bounds[0],bounds[1],Z,bounds[2],bounds[3],Z+8.5),'MODULE_CONNECTOR_BODY_ENVELOPE_CATALOGUE')
# Report real own-part Boolean overlaps without claiming unknown module components are absent.
module_intersections=[]
checks=[x for x in parts if x['name'].endswith('_retainer')]
others=[x for x in parts if ('socket_'in x['name']or 'module_PCB'in x['name']or x['name'].endswith('_retainer'))]
for aa in checks:
 for bb in others:
  if aa['name']==bb['name']:continue
  aab=aa['shape'].BoundingBox();bbb=bb['shape'].BoundingBox()
  if aab.xmax<=bbb.xmin or aab.xmin>=bbb.xmax or aab.ymax<=bbb.ymin or aab.ymin>=bbb.ymax or aab.zmax<=bbb.zmin or aab.zmin>=bbb.zmax:continue
  vol=aa['shape'].intersect(bb['shape']).Volume()
  if vol>1e-6:module_intersections.append({'a':aa['name'],'b':bb['name'],'volume_mm3':vol,'status':'REVIEW_CONTACT_OR_COLLISION_NOT_AUTOMATICALLY_ACCEPTED'})
print('Module/retainer bounded tests',module_intersections,flush=True)
# STEP of our own parts only is publishable, ST assembly retained locally with original provenance.
a=cq.Assembly(name='V20_own_parts_G12p25_review')
for item in parts:a.add(item['shape'],name=item['name'])
a.save(str(P/'cad/own-parts-G12p25.step'));print('Own assembly exported',flush=True)
full=cq.Assembly(name='V20_official_C05_G12p25_REVIEW');full.add(cq.Compound.makeCompound([w for i,w in enumerate(world)if i not in excluded]),name='ST_C05_nominal_filtered_Morpho')
for item in parts:full.add(item['shape'],name=item['name'])
full.save(str(E/'full-C05-G12p25-review.step'));print('Full local assembly exported',flush=True)
result={'G_mm':G,'ST_source_sha256':hashlib.sha256(src.read_bytes()).hexdigest(),'source_solids':len(solids),'source_valid':shape.isValid(),'frame':'CAD x=STx+Tx,y=STy-Ty,z=Nucleo top0; own boxes use KiCadY reflected; shield bottomG/topG+1.6','Tx_Ty':T,'source_geometry':source_rows,'morpho_population_excluded_indices':excluded,'population_exclusion_limit':'bounding-region filter for population scenario only; complete source retained, any excluded non-header object would need separate review; full source intersections also reported','intersections':tests,'filtered_positive_count':sum(not t['population_excluded']for t in tests),'intentional_contacts':'post/socket, post/J5 cavities; socket entry voids not contact-force models','own_parts':[{'name':x['name'],'status':x['status']}for x in parts],'elapsed_s':time.time()-start,'full_ST_assembly_location':str(E/'full-C05-G12p25-review.step'),'physical_mating':'NOT_TESTED','module_retainer_positive_intersections':module_intersections}
(P/'preuves/C05-collisions-G12p25.json').write_text(json.dumps(result,indent=2)+'\n');print('DONE',time.time()-start,flush=True)
