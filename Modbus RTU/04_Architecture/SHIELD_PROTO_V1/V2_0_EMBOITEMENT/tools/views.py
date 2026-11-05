from pathlib import Path
import json
import matplotlib;matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from matplotlib.backends.backend_pdf import PdfPages
P=Path(__file__).resolve().parents[1];R=P.parent;T=json.loads((P/'preuves/tolerances.json').read_text());I=json.loads((R/'V1_9_C05/preuves/interfaces-ST.json').read_text());G=12.25
with PdfPages(P/'cad/assemblage-et-coupes.pdf')as pdf:
 fig,ax=plt.subplots(figsize=(8.27,11.69));poly=I['Nucleo_outline_shield_frame_mm'];ax.plot([a[0]for a in poly],[a[1]for a in poly],color='gray',label='ST C05 outline')
 ax.add_patch(Rectangle((20,17),90,100,fill=False,edgecolor='green',lw=2,label='Shield V19 unchanged XY'))
 B=json.loads((R/'V1_9_C05/docs/checks/pcb-readback.json').read_text())
 for p in B['pads']:
  if p['ref']in ['J1','J2','J3','J4','J5']:ax.plot(p['x'],p['y'],'.',color='navy',ms=3)
 for c in I['components']:
  x,y=c['xy_shield_mm'];ax.plot(x,y,'o',ms=3);ax.annotate(c['ref'],(x,y),xytext=(3,-8),textcoords='offset points',fontsize=7)
 for x,y in json.loads((R/'V1_9_C05/design.json').read_text())['mounting_holes_mm']:ax.add_patch(plt.Circle((x,y),1.75,fill=False,color='orange'))
 ax.set_aspect('equal');ax.invert_yaxis();ax.grid();ax.set_xlabel('KiCad X mm');ax.set_ylabel('KiCad Y mm');ax.set_title('V2.0 — genuine C05 frame / V19 XY retained\nG12.25 scenario, no physical qualification');ax.legend(loc='lower right');fig.text(.1,.05,'Non-print template. No new paper requested.\nCAD assembly is a nominal review; contact forces and actual cables unknown.',fontsize=9);pdf.savefig(fig);fig.savefig(P/'cad/vue-dessus.png',dpi=130);plt.close(fig)
 fig,axs=plt.subplots(1,2,figsize=(11.69,8.27))
 for ax,title,b,h,tip in [(axs[0],'Zio / Connfly type1',2.5,8.51,G-2.5-6.1),(axs[1],'J5 / existing CN12',2.4,8.51,8.22)]:
  ax.add_patch(Rectangle((0,-1.6),6,1.6,color='steelblue',label='Nucleo PCB'))
  ax.add_patch(Rectangle((0,G),6,1.6,color='green',label='Shield1.6'))
  if ax==axs[0]:
   ax.add_patch(Rectangle((1,0),4,8.51,color='gray',alpha=.5));ax.add_patch(Rectangle((1,G-2.5),4,2.5,color='black',alpha=.7));ax.plot([3,3],[tip,G-2.5],color='orange',lw=3)
  else:
   ax.add_patch(Rectangle((1,0),4,2.4,color='black',alpha=.7));ax.add_patch(Rectangle((1,G-8.51),4,8.51,color='gray',alpha=.5));ax.plot([3,3],[2.4,8.22],color='orange',lw=3)
  sc=T['scenarios']['12.25'];prefix='Zio'if ax==axs[0]else'J5';ax.text(.2,15,f"I nominal={sc[prefix+'_insertion_nominal']:.2f}mm\nWorst model I={sc[prefix+'_insertion_worst_range'][0]:.2f}..{sc[prefix+'_insertion_worst_range'][1]:.2f}\nMin body gap={sc[prefix+'_min_gap']:.2f}mm",fontsize=9)
  ax.set_xlim(-.5,7);ax.set_ylim(-2,19);ax.set_title(title);ax.set_ylabel('Z above Nucleo top / mm');ax.grid(axis='y');ax.set_xticks([])
 fig.suptitle('Dimensioned envelope sections — G12.25 ±0.10 conditional; not exact contact geometry')
 fig.text(.1,.04,'±0.10 CN12 height uncertainty and spacer/assembly budget are assumptions.\nNo full tolerance guarantee; no preload/forced seating. Catalogue insertion3.68..6.35mm.',fontsize=10);pdf.savefig(fig);fig.savefig(P/'cad/coupes-Z.png',dpi=130);plt.close(fig)
print('PDF2 pages and2PNG envelope views generated; no physical qualification')
