from pathlib import Path
import json,numpy as np
import matplotlib;matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Wedge,Rectangle
from matplotlib.backends.backend_pdf import PdfPages
P=Path(__file__).resolve().parents[1]
with PdfPages(P/'cad/plan-et-coupe.pdf')as pdf:
 fig,axs=plt.subplots(1,2,figsize=(11.69,8.27));ax=axs[0];ax.add_patch(Wedge((0,0),3,180,360,width=1.8,color='teal'));ax.axhline(0,color='black',ls=':');ax.arrow(0,1,0,1.2,head_width=.25,color='red');ax.text(-2.9,2.6,'Opening toward adjacent header');ax.set_xlim(-4,4);ax.set_ylim(-4,4);ax.set_aspect('equal');ax.grid();ax.set_title('D126 section / mm\nOD6, bore2.4, half annulus');ax.text(-3.8,-3.7,'H12.25 ±0.10; radial ligament1.8mm',fontsize=9)
 ax=axs[1];ax.add_patch(Rectangle((-3,-1.6),6,1.6,color='navy'));ax.add_patch(Rectangle((-3,0),6,12.25,color='teal',alpha=.7));ax.add_patch(Rectangle((-3,12.25),6,1.6,color='green'));ax.add_patch(Rectangle((-1,-1.6),2,20,color='gray'));ax.add_patch(Rectangle((-1.9,-2.9),3.8,1.3,color='gray'))
 for z in [13.85,15.45]:ax.add_patch(Rectangle((-2,z),4,1.6,fill=False,ec='black'))
 ax.set_ylim(-4,21);ax.set_xlim(-5,7);ax.grid(axis='y');ax.set_xticks([]);ax.set_title('Stack envelope / Z mm\nNucleo top=0');ax.text(2.5,14.4,'2 nuts\non top',fontsize=8);ax.text(2.5,5,'G=12.25',fontsize=8);ax.text(-4.7,-3.7,'Head below Nucleo',fontsize=8)
 fig.suptitle('V2.1 REVIEW ONLY / No PCB change / No physical qualification',fontsize=13);fig.text(.07,.04,'Envelope view. Nylon fasteners, support print/process and clamping NOT QUALIFIED.\nOpening orientation: south at top anchor, north at two lower anchors. Keep within5deg.',fontsize=10);pdf.savefig(fig);fig.savefig(P/'cad/vue-fixations.png',dpi=130);plt.close(fig)
print('PDF1 page and PNG generated')
