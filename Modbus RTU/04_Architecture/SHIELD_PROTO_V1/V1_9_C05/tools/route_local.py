"""Conservative deterministic two-layer grid router for this small passive carrier.
Route results require KiCad DRC; a routed JSON is never a fabrication release.
Dependencies: numpy, shapely. No hardware access.
"""
from pathlib import Path
import json,heapq,math,time,sys
import numpy as np
from shapely.geometry import Point,LineString
from shapely import union_all, contains_xy
P=Path(__file__).resolve().parents[1];G=json.loads((P/'docs/pcb_geometry.json').read_text());grid=.1
bx0=min(p[0]for p in G["outline"]);by0=min(p[1]for p in G["outline"]);bx1=max(p[0]for p in G["outline"]);by1=max(p[1]for p in G["outline"]);xmin,ymin=bx0-1,by0-1;xmax,ymax=bx1+1,by1+1;W=int((xmax-xmin)/grid)+1;H=int((ymax-ymin)/grid)+1
names=sorted({p['net']for p in G['pads']if p['net']and not p['net'].startswith('unconnected-')});ids={n:i+1 for i,n in enumerate(names)};owner=np.zeros((2,H,W),np.int16)
def ij(x,y):return int(round((x-xmin)/grid)),int(round((y-ymin)/grid))
def xy(i,j):return [round(xmin+i*grid,5),round(ymin+j*grid,5)]
def disk(x,y,r,net,layers=(0,1)):
 i,j=ij(x,y);k=math.ceil(r/grid)+1;x0=max(i-k,0);x1=min(i+k+1,W);y0=max(j-k,0);y1=min(j+k+1,H)
 yy,xx=np.ogrid[y0:y1,x0:x1];mask=((xmin+xx*grid-x)**2+(ymin+yy*grid-y)**2)<=r*r
 for l in layers:owner[l,y0:y1,x0:x1][mask]=net
# Outside edge remains an impassable obstacle on both layers, with margin.
for j in range(H):
 for i in range(W):
  x,y=xy(i,j)
  if x<bx0+.6 or x>bx1-.6 or y<by0+.6 or y>by1-.6:owner[:,j,i]=-1
geometries={}
pads={}
for p in G['pads']:
 net=ids.get(p['net'],-1);disk(p['x'],p['y'],p['diameter']/2,net)
 geometries.setdefault(net,[]).append(Point(p['x'],p['y']).buffer(p['diameter']/2,quad_segs=24))
 if p['net']and not p['net'].startswith('unconnected-'):pads.setdefault(p['net'],[]).append(p)

def route(start,end,net):
 xx,yy=np.meshgrid(xmin+np.arange(W)*grid,ymin+np.arange(H)*grid)
 block=[];via_block=[]
 for layer in range(2):
  obs=union_all([g for n,gs in geometries.items()if n!=net for g,ls in [(g,(0,1))if not isinstance(g,tuple)else g for g in gs]if layer in ls])
  block.append(contains_xy(obs.buffer(.36),xx,yy)|(xx<bx0+.6)|(xx>bx1-.6)|(yy<by0+.6)|(yy>by1-.6))
  via_block.append(contains_xy(obs.buffer(.58),xx,yy))
 block=np.array(block);via_ok=~(via_block[0]|via_block[1])
 sx,sy=ij(start['x'],start['y']);ex,ey=ij(end['x'],end['y']);cost=np.full((2,H,W),np.inf,np.float32);par=np.full((2,H,W),-1,np.int32)
 def encode(l,x,y):return(l*H+y)*W+x
 def decode(k):x=k%W;k//=W;return k//H,x,k%H
 def heuristic(x,y):dx=abs(ex-x);dy=abs(ey-y);return max(dx,dy)+.41421356237*min(dx,dy)
 heap=[]
 for l in ([start['layer']] if 'layer' in start else range(2)):cost[l,sy,sx]=0;heapq.heappush(heap,(heuristic(sx,sy),0,l,sx,sy))
 endstate=None;visits=0
 while heap:
  _,g,l,x,y=heapq.heappop(heap)
  if g>float(cost[l,y,x])+.001:continue
  visits+=1
  if x==ex and y==ey:endstate=(l,x,y);break
  for dx,dy in[(1,0),(-1,0),(0,1),(0,-1),(1,1),(1,-1),(-1,1),(-1,-1)]:
   nx,ny=x+dx,y+dy
   if not(0<=nx<W and 0<=ny<H)or block[l,ny,nx]:continue
   if dx and dy and(block[l,y,nx]or block[l,ny,x]):continue
   ng=g+(1.41421356237 if dx and dy else 1)
   if ng+.001<float(cost[l,ny,nx]):cost[l,ny,nx]=ng;par[l,ny,nx]=encode(l,x,y);heapq.heappush(heap,(ng+heuristic(nx,ny),ng,l,nx,ny))
  nl=1-l
  if via_ok[y,x] and not block[nl,y,x]:
   ng=g+24
   if ng+.001<float(cost[nl,y,x]):cost[nl,y,x]=ng;par[nl,y,x]=encode(l,x,y);heapq.heappush(heap,(ng+heuristic(x,y),ng,nl,x,y))
 if endstate is None:raise RuntimeError(f'No safe route {start["ref"]}.{start["number"]} to {end["ref"]}.{end["number"]}, visits={visits}')
 path=[];l,x,y=endstate
 while True:
  path.append((l,x,y));k=int(par[l,y,x])
  if k<0:break
  l,x,y=decode(k)
 path.reverse();points=[{'layer':path[0][0],'xy':[start['x'],start['y']]}]+[{'layer':l,'xy':xy(x,y)}for l,x,y in path]+[{'layer':path[-1][0],'xy':[end['x'],end['y']]}]
 compact=[points[0]]
 for pt in points[1:]:
  if pt!=compact[-1]:compact.append(pt)
 simplified=[]
 for pt in compact:
  while len(simplified)>=2:
   a,z=simplified[-2:]
   if a['layer']!=pt['layer']or z['layer']!=pt['layer']:break
   ax,ay=a['xy'];zx,zy=z['xy'];px,py=pt['xy'];cross=(zx-ax)*(py-zy)-(zy-ay)*(px-zx)
   if abs(cross)>1e-7:break
   simplified.pop()
  simplified.append(pt)
 for a,z in zip(compact,compact[1:]):
  if a['layer']!=z['layer']:
   disk(*a['xy'],.325,net);geometries.setdefault(net,[]).append(Point(*a['xy']).buffer(.325,quad_segs=24))
  else:
   geometries.setdefault(net,[]).append((LineString([a['xy'],z['xy']]).buffer(.125,quad_segs=24),(a['layer'],)))
   length=math.dist(a['xy'],z['xy']);n=max(1,math.ceil(length/.08))
   for i in range(n+1):disk(a['xy'][0]+(z['xy'][0]-a['xy'][0])*i/n,a['xy'][1]+(z['xy'][1]-a['xy'][1])*i/n,.125,net,(a['layer'],))
 return simplified,visits


# V1.9: preserve all original routes and all prefixes outside CN7 escape area.
# Register their copper as obstacles, then reroute only six CN7 tails.
import copy
baseline=json.loads((P.parent/'cad/routes.json').read_text());updated=copy.deepcopy(baseline);jobs=[];prefixes={}
def register(points,net):
 for a,z in zip(points,points[1:]):
  if a['layer']!=z['layer']:
   geometries.setdefault(net,[]).append(Point(*a['xy']).buffer(.325,quad_segs=24))
  elif a['xy']!=z['xy']:geometries.setdefault(net,[]).append((LineString([a['xy'],z['xy']]).buffer(.125,quad_segs=24),(a['layer'],)))
for i,r in enumerate(updated['routes']):
 if r['to'].startswith('J1.'):
  cut=next(k for k,t in enumerate(r['points']) if t['xy'][0]>73 and t['xy'][1]<53)
  assert cut>0;prefix=copy.deepcopy(r['points'][:cut]);register(prefix,ids[r['net']]);prefixes[i]=prefix;jobs.append(i)
 else:
  if r['to']=='J5.5':
   for t in r['points'][10:]:t['xy'][1]=round(t['xy'][1]+.025001,6)
  register(r['points'],ids[r['net']])
started=time.monotonic()
for i in jobs:
 r=updated['routes'][i];prefix=prefixes[i];a=prefix[-1];end=next(q for q in G['pads'] if q['ref']+'.'+q['number']==r['to'])
 tail,visits=route({'x':a['xy'][0],'y':a['xy'][1],'layer':a['layer']},end,ids[r['net']])
 if tail[0]['layer']!=a['layer']:
  # PTH-free splice requires explicit via if layer differs.
  tail.insert(0,copy.deepcopy(a))
 r['points']=prefix[:-1]+tail
 print(r['net'],'retained prefix',len(prefix),'tail',len(tail),'visits',visits,flush=True)
updated['elapsed_s']=time.monotonic()-started;updated['V19_scope']='six CN7 local tails +J5 endpoint25um; all other routes retained'
(P/'cad/routes.json').write_text(json.dumps(updated,indent=2)+'\n');print('local routing complete',updated['elapsed_s'],flush=True)
