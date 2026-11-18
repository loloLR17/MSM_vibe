"""Deterministic local reroute: five terminal branches, fixed layers, no new vias.

Reads actual native pads/tracks, not a pinout matrix. Requires later independent
M-01, native DRC and copper topology audit; routing alone is never a PASS.
"""
from pathlib import Path
import json,math,heapq,uuid,time
import numpy as np
import sexpdata as sx
from shapely.geometry import Point,LineString
from shapely import union_all,contains_xy
P=Path(__file__).resolve().parents[1];S=sx.Symbol
T=lambda n,t:isinstance(n,list)and bool(n)and str(n[0])==t
one=lambda n,t:next(x for x in n if T(x,t))
f=P/'cad/TR2_Shield_Proto_V1.kicad_pcb';board=sx.loads(f.read_text());pads={};obs=[]
# All board footprints here have zero rotation; preserve exact native positions.
for fp in board:
 if not T(fp,'footprint'):continue
 at=one(fp,'at');assert len(at)==3 or float(at[3])==0
 ref=next(x[2]for x in fp if T(x,'property')and x[1]=='Reference')
 for pad in fp:
  if not T(pad,'pad'):continue
  pos=one(pad,'at');xy=[float(at[1])+float(pos[1]),float(at[2])+float(pos[2])]
  size=one(pad,'size');assert size[1]==size[2]
  net=next((x[1]for x in pad if T(x,'net')),'NPTH')
  pads[ref+'.'+str(pad[1])]=dict(xy=xy,net=net)
  obs.append((net,(0,1),Point(xy).buffer(float(size[1])/2,quad_segs=32)))
for n in board:
 if T(n,'segment'):
  net=one(n,'net')[1];layer=0 if one(n,'layer')[1]=='F.Cu'else 1
  obs.append((net,(layer,),LineString([one(n,'start')[1:],one(n,'end')[1:]]).buffer(float(one(n,'width')[1])/2,quad_segs=32)))
 elif T(n,'via'):
  obs.append((one(n,'net')[1],(0,1),Point(one(n,'at')[1:]).buffer(float(one(n,'size')[1])/2,quad_segs=32)))
# The existing SCK via and all other holes remain where V2.3 placed them.
pads['VIA_ACC_SCK']=dict(xy=[49.8,37.5],net='ACC_SCK')
grid=.1;xmin,ymin=20.,17.;W,H=901,1001
xx,yy=np.meshgrid(xmin+np.arange(W)*grid,ymin+np.arange(H)*grid)

def index(p):return int(round((p[0]-xmin)/grid)),int(round((p[1]-ymin)/grid))
def pos(x,y):return [round(xmin+x*grid,6),round(ymin+y*grid,6)]
def route(a,b,layer,net):
 start,end=pads[a]['xy'],pads[b]['xy'];sx_,sy_=index(start);ex,ey=index(end)
 obstacles=union_all([shape for n,layers,shape in obs if n!=net and layer in layers])
 block=contains_xy(obstacles.buffer(.37),xx,yy)|(xx<20.6)|(xx>109.4)|(yy<17.6)|(yy>116.4)
 assert not block[sy_,sx_] and not block[ey,ex],(a,b,'blocked endpoint')
 cost=np.full((H,W),np.inf);parent=np.full((H,W),-1,np.int32);cost[sy_,sx_]=0
 def h(x,y):dx,dy=abs(ex-x),abs(ey-y);return max(dx,dy)+.41421356237*min(dx,dy)
 heap=[(h(sx_,sy_),0,sx_,sy_)];visits=0
 while heap:
  _,g,x,y=heapq.heappop(heap)
  if g>cost[y,x]+1e-9:continue
  visits+=1
  if (x,y)==(ex,ey):break
  for dx,dy in[(1,0),(-1,0),(0,1),(0,-1),(1,1),(1,-1),(-1,1),(-1,-1)]:
   nx,ny=x+dx,y+dy
   if not(0<=nx<W and 0<=ny<H)or block[ny,nx]:continue
   if dx and dy and(block[y,nx]or block[ny,x]):continue
   ng=g+(math.sqrt(2)if dx and dy else 1)
   if ng+1e-9<cost[ny,nx]:cost[ny,nx]=ng;parent[ny,nx]=y*W+x;heapq.heappush(heap,(ng+h(nx,ny),ng,nx,ny))
 else:raise RuntimeError(('No route at frozen geometry/layer',a,b,visits))
 points=[];x,y=ex,ey
 while True:
  points.append(pos(x,y));previous=int(parent[y,x])
  if previous<0:break
  y,x=divmod(previous,W)
 points=list(reversed(points));points=[start]+points+[end]
 compact=[]
 for p in points:
  if compact and math.dist(p,compact[-1])<1e-6:continue
  while len(compact)>1:
   u,v=compact[-2:];cross=(v[0]-u[0])*(p[1]-v[1])-(v[1]-u[1])*(p[0]-v[0])
   if abs(cross)>1e-8:break
   compact.pop()
  compact.append(p)
 # Check the actual compact straight segments, not only their grid vertices.
 for u,v in zip(compact,compact[1:]):
  geom=LineString([u,v]);assert geom.distance(obstacles)>=.324,('Clearance',a,b,geom.distance(obstacles))
  obs.append((net,(layer,),geom.buffer(.125,quad_segs=32)))
  node=[S('segment'),[S('start'),*u],[S('end'),*v],[S('width'),.25],[S('layer'),'F.Cu'if layer==0 else'B.Cu'],[S('net'),net],[S('uuid'),str(uuid.uuid5(uuid.NAMESPACE_URL,'tr2-v24-'+repr((net,layer,u,v))))]]
  board.append(node)
 print(net,a,b,len(compact)-1,'segments',visits,'visits',flush=True)
 return dict(net=net,**{'from':a,'to':b},points=[dict(layer=layer,xy=p)for p in compact])

start=time.monotonic();new=[]
for net,a,b,layer in[
 ('ACC_CS','R1.2','J8.19',0),
 ('ACC_SCK','TP3.1','J8.20',0),
 ('ACC_SCK','VIA_ACC_SCK','J8.20',1),
 ('ACC_MISO','J2.8','J8.22',1),
 ('ACC_MOSI','J2.10','J8.21',1)]:new.append(route(a,b,layer,net))
f.write_text(sx.dumps(board)+'\n')
routes=json.loads((P.parent/'V2_3_ARNACO/cad/routes.json').read_text());kept=[]
for r in routes['routes']:
 if r['net'].startswith('ACC_')and(r['from'].startswith('J8.')or r['to'].startswith('J8.')):
  if r['net']=='ACC_SCK'and r['to']=='J2.6':
   idx=next(i for i,p in enumerate(r['points'])if p['layer']==0)
   kept.append(dict(net='ACC_SCK',**{'from':'VIA_ACC_SCK','to':'J2.6'},points=r['points'][idx:]))
 else:kept.append(r)
routes['routes']=kept+new;routes['elapsed_s']=time.monotonic()-start;routes['local_correction']='V2.4 five SPI3 terminal branches; existing four vias fixed; all other tracks unchanged'
(P/'cad/routes.json').write_text(json.dumps(routes,indent=2)+'\n')
(P/'preuves/routage-local.json').write_text(json.dumps({'new_routes':new,'elapsed_s':routes['elapsed_s'],'new_vias':0,'deleted_vias':0,'native_DRC_required':True},indent=2)+'\n')
