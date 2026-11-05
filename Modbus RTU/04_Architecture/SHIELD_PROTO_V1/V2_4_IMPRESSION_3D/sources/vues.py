"""Assembly/section views from own exported STEP, with labeled reference envelopes."""
from pathlib import Path
import json
import cadquery as cq
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.colors import to_rgb
from matplotlib.backends.backend_pdf import PdfPages
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import modele as m
P=m.P
def triangles(s):
    vv,ff=s.tessellate(.12,.25);v=np.array([q.toTuple()for q in vv]);return v[np.array(ff)]
def main():
    refs,_=m.build()
    own=[(r['label'],r['shape'],'#748a9b'if r['part']=='01_base'else'#e8a847'if r['part']=='02_couvercle_ARNACO'else'#ca7047'if r['part'].startswith(('16','17'))else'#3baca0')for r in m.INST]
    env=[(n,s,'#56704d'if'PCB'in n or'shield'in n or'Nucleo'in n else'#444444')for n,s in refs if n!='PC_window_not_printed']
    # Own geometry is identical to exported models. Check before drawing.
    for n,r in m.PARTS.items():
        actual=cq.importers.importStep(str(P/'cad'/(n+'.step'))).val()
        assert abs(actual.Volume()-r['shape'].Volume())<1e-5
    data=[(n,triangles(s),c)for n,s,c in own+env]
    def draw3(name,mode):
        fig=plt.figure(figsize=(11.69,8.27));ax=fig.add_subplot(111,projection='3d')
        tris=[];colors=[]
        for n,tri,col in data:
            if mode=='ouvert' and n.startswith(('02_','12_')):continue
            if mode=='ferme' and any(n==a for a,_,_ in env):continue
            t=tri.copy()
            if mode=='eclate':
                if n.startswith(('02_','12_')):t[:,:,2]+=65
                elif n.startswith(('03_','07_','10_')):t[:,:,1]+=30
                elif n.startswith(('04_','08_')):t[:,:,1]-=30
                elif n.startswith(('05_','09_')):t[:,:,0]-=25
                elif n.startswith('06_'):t[:,:,0]+=25
            normal=np.cross(t[:,1]-t[:,0],t[:,2]-t[:,0]);normal/=np.maximum(np.linalg.norm(normal,axis=1)[:,None],1e-12)
            light=np.array([-.4,.3,1]);light/=np.linalg.norm(light)
            shade=.55+.45*np.maximum(normal@light,0)
            camera=np.array([np.cos(np.deg2rad(38))*np.cos(np.deg2rad(-125)),np.cos(np.deg2rad(38))*np.sin(np.deg2rad(-125)),np.sin(np.deg2rad(38))])
            visible=normal@camera>1e-7
            tris.extend(t[visible]);colors.extend((shade[:,None]*np.array(to_rgb(col)))[visible])
        ax.add_collection3d(Poly3DCollection(tris,facecolors=colors,edgecolors='none',linewidths=0,alpha=1))
        ax.set_xlim(-20,180);ax.set_ylim(-180,55);ax.set_zlim(-15,125 if mode=='eclate'else 60)
        ax.set_box_aspect((200,235,140 if mode=='eclate'else 90));ax.view_init(38,-125)
        ax.set_xlabel('X CAD / mm');ax.set_ylabel('Y CAD = -Y KiCad / mm');ax.set_zlabel('Z / mm')
        ax.set_title('TR2 V2.4 — '+mode+' — géométrie nominale\nOrange : couvercle ARNACO ; rouille : étriers réservés, hors ZIP')
        fig.text(.07,.025,'Enveloppes modules provisoires, pas des modèles de réception. Ni montage réel ni résistance/optique qualifiés.',fontsize=9)
        fig.savefig(P/'vues'/(name+'.png'),dpi=155);return fig
    with PdfPages(P/'vues/ASSEMBLAGE_ET_COUPES.pdf')as pdf:
        for n,mode in [('01_ensemble_ouvert','ouvert'),('02_ferme_ARNACO','ferme'),('03_eclate','eclate')]:
            f=draw3(n,mode);pdf.savefig(f);plt.close(f)
        # Actual solid sections, not just bounding boxes. Each thin slab is
        # projected on its mid-plane, 0.04mm maximum section thickness.
        fig,axes=plt.subplots(2,1,figsize=(11.69,8.27))
        for ax,plane in zip(axes,['X60','Y92']):
            slab=m.box(59.98,-30,-15,60.02,150,55)if plane=='X60'else m.box(0,91.98,-15,180,92.02,55)
            for n,s,c in own+env:
                cut=s.intersect(slab)
                if cut.Volume()<1e-7:continue
                for face in cut.Faces():
                    normal=face.normalAt()
                    if abs(normal.x if plane=='X60'else normal.y)<.99:continue
                    for wire in face.Wires():
                        for edge in wire.Edges():
                            pts=np.array([v.toTuple()for v in edge.sample(16)[0]])
                            if len(pts):
                                xx=-pts[:,1]if plane=='X60'else pts[:,0]
                                ax.plot(xx,pts[:,2],'-',lw=.8,color=c)
            ax.set_xlim(-22,169);ax.set_ylim(-15,53);ax.set_aspect('equal');ax.grid(alpha=.25)
            ax.set_xlabel('KiCad Y / mm'if plane=='X60'else'KiCad X / mm');ax.set_ylabel('Z / mm')
            ax.set_title('Coupe '+plane+' — plancher -11 ; Nucleo 0 ; shield 12,25 / 13,85 ; toit 47 / 50 ; relief 51')
        fig.suptitle('Sections des solides propres et enveloppes nominales — G=12,25 mm')
        fig.tight_layout();pdf.savefig(fig);fig.savefig(P/'vues/04_coupes.png',dpi=155);plt.close(fig)
        # Top view of actual lid: embossed contours, not typeset replacement text.
        lid=m.PARTS['02_couvercle_ARNACO']['shape'];fig,ax=plt.subplots(figsize=(8.27,8.27))
        for edge in lid.Edges():
            pts=np.array([v.toTuple()for v in edge.sample(10)[0]])
            if len(pts):ax.plot(pts[:,0],-pts[:,1],color='#172e41',lw=.55)
        ax.set_aspect('equal');ax.invert_yaxis();ax.set_title('Couvercle réel exporté — ARNACO monobloc +1 mm\nAucune police ni pièce logo à fournir à l’imprimeur')
        ax.set_xlabel('X KiCad / mm');ax.set_ylabel('Y KiCad / mm');ax.grid(alpha=.15)
        fig.tight_layout();pdf.savefig(fig);fig.savefig(P/'vues/05_relief_reel.png',dpi=155);plt.close(fig)
    print('Assembly PDF and five PNG views generated',flush=True)
if __name__=='__main__':main()
