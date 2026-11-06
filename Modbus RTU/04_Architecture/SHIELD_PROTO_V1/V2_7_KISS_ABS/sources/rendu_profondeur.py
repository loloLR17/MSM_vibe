"""Offscreen VTK depth-buffer rendering of CAD triangles; no image retouching."""
import numpy as np
import math
import vtkmodules.all as vtk
from vtkmodules.util.numpy_support import numpy_to_vtk,numpy_to_vtkIdTypeArray,vtk_to_numpy
import matplotlib.pyplot as plt

def render(gg,mode,path,triangles,explode_offset):
 exploded=mode=='exploded';width,height=(1100,1800)if exploded else(1600,1200)
 ren=vtk.vtkRenderer();ren.SetBackground(1,1,1);win=vtk.vtkRenderWindow();win.SetOffScreenRendering(1);win.SetSize(width,height);win.SetMultiSamples(4);win.AddRenderer(ren)
 ptsall=[];centres={}
 for i,g in gg.items():
  if mode=='open'and i in[11,12]:continue
  centres[i]=[]
  for row in g['items']:
   off=np.array(explode_offset(g,row)if exploded else(0,0,0));tr=triangles(row['shape'])+off;pts=tr.reshape(-1,3);n=len(tr);ptsall.append(pts)
   points=vtk.vtkPoints();points.SetData(numpy_to_vtk(pts,deep=True));cells=vtk.vtkCellArray();cells.SetData(numpy_to_vtkIdTypeArray(np.arange(n+1,dtype=np.int64)*3,deep=True),numpy_to_vtkIdTypeArray(np.arange(n*3,dtype=np.int64),deep=True));poly=vtk.vtkPolyData();poly.SetPoints(points);poly.SetPolys(cells)
   normals=vtk.vtkPolyDataNormals();normals.SetInputData(poly);normals.ComputePointNormalsOff();normals.ComputeCellNormalsOn();normals.ConsistencyOff()
   mapper=vtk.vtkPolyDataMapper();mapper.SetInputConnection(normals.GetOutputPort());actor=vtk.vtkActor();actor.SetMapper(mapper);prop=actor.GetProperty();prop.SetColor(*row['color']);prop.SetAmbient(.30);prop.SetDiffuse(.70);prop.SetSpecular(.1);prop.SetInterpolationToFlat()
   if 'window'in row['name']:prop.SetOpacity(.2)
   elif exploded and i==1:prop.SetOpacity(.6)
   ren.AddActor(actor);b=row['shape'].BoundingBox();centres[i].append(np.array([(b.xmin+b.xmax)/2,(b.ymin+b.ymax)/2,(b.zmin+b.zmax)/2])+off)
 pts=np.concatenate(ptsall);az=math.radians(-62 if exploded else -59);el=math.radians(13 if exploded else 42)
 right=np.array([-math.sin(az),math.cos(az),0]);up=np.array([-math.sin(el)*math.cos(az),-math.sin(el)*math.sin(az),math.cos(el)]);d=np.array([math.cos(el)*math.cos(az),math.cos(el)*math.sin(az),math.sin(el)])
 xx=pts@right;yy=pts@up;dd=pts@d;centre=right*(xx.min()+xx.max())/2+up*(yy.min()+yy.max())/2+d*(dd.min()+dd.max())/2
 cam=ren.GetActiveCamera();cam.SetPosition(*(centre+d*1000));cam.SetFocalPoint(*centre);cam.SetViewUp(0,0,1);cam.ParallelProjectionOn();cam.SetParallelScale(max((yy.max()-yy.min())/2,(xx.max()-xx.min())/2/(width/height))*1.08);ren.ResetCameraClippingRange();win.Render()
 capture=vtk.vtkWindowToImageFilter();capture.SetInput(win);capture.ReadFrontBufferOff();capture.Update();im=capture.GetOutput();pixels=vtk_to_numpy(im.GetPointData().GetScalars()).reshape(height,width,-1)[::-1]
 f=plt.figure(figsize=(10,12)if exploded else(10,8));ax=f.add_axes([0,.045,.74,.9]if exploded else[0,.045,1,.88]);ax.imshow(pixels);ax.axis('off')
 if exploded:
  for i,cc in centres.items():
   c=np.mean(cc,axis=0)
   if i==1:c[2]=-8
   if i==2:c[2]=26
   ren.SetWorldPoint(*c,1);ren.WorldToDisplay();x,y,_=ren.GetDisplayPoint();y=height-y
   ax.annotate(str(i),xy=(x,y),xytext=(30,y),fontsize=9,fontweight='bold',ha='center',va='center',bbox={'boxstyle':'circle,pad=.25','fc':'white','ec':'#44566b'},arrowprops={'arrowstyle':'-','color':'#44566b','lw':.7})
  for j,g in gg.items():f.text(.75,.92-(j-1)*.06,f"{j:02d}  {g['label']}",fontsize=8.4,va='top')
  f.text(.75,.15,'Un seul prototype utile.\nVariantes / réserves absentes.\nEnveloppes fabricants citées ;\naucune qualification physique.',fontsize=8,va='top')
 title='TR2 V2.7 — un prototype, vue éclatée complète'if exploded else'TR2 V2.7 — boîtier fermé, USER sur le capot'if mode=='closed'else'TR2 V2.7 — implantation, capot et USER retirés pour lecture'
 f.suptitle(title,fontsize=14,fontweight='bold',y=.975);f.text(.035,.02,'Projection de la CAO, profondeur calculée. Couleurs de lecture ; ABS monocolore. Ajustage à froid.',fontsize=8,color='#55606d');f.savefig(path,dpi=190,facecolor='white');plt.close(f);win.Finalize();print('Depth rendering',mode,flush=True)
