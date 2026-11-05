"""Short assembly diagrams, not a new enclosure design or physical drawing."""
import json
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle,Polygon
from matplotlib.backends.backend_pdf import PdfPages
import controle as c
P=c.P
with PdfPages(P/'vues/SCHEMAS_MONTAGE_V25.pdf')as pdf:
    fig,ax=plt.subplots(figsize=(8.27,11.69))
    ax.add_patch(Rectangle((8,-20),158,166,fill=False,lw=2))
    d=json.loads((c.V/'sources/geometrie-reference.json').read_text());ax.add_patch(Polygon(d['Nucleo_outline'],facecolor='#d9ead3',edgecolor='#648056',alpha=.65))
    ax.add_patch(Rectangle((20,17),90,100,fill=False,ls='--',ec='#5577aa'))
    for index,(name,p,u,a)in enumerate(c.mounts(),1):
        x,y=p[0],-p[1];ax.plot(x,y,'o',ms=5,color='#b65b35');ax.annotate(str(index),(x,y),(0,6),textcoords='offset points',fontsize=9,ha='center')
    for y in [21.192131,97.39]:ax.plot([5,11],[y,y],color='#3868c4',lw=4)
    ax.set_xlim(-4,182);ax.set_ylim(158,-30);ax.set_aspect('equal');ax.grid(alpha=.2)
    ax.set_xlabel('X KiCad / mm');ax.set_ylabel('Y KiCad / mm')
    ax.set_title('V2.5 — 17 fixations M3×12 par coffret\nProjection des axes, pas preuve de distance en Z')
    fig.text(.07,.04,'1–4 couvercle ; 5–7 bras Nucleo ; 8–9 trappe outil ; 10–11 bride RS485.\n12–13 vitre ; 14–15 bride nappe ; 16 USER ; 17 SD.\nVert : Nucleo ; pointillé : shield. Bleu : accès écrous.\nCharger 9 écrous captifs avant les cartes. D126 : nylon M2 acheté.\nAppuis modules et quatre étriers non libérés.',fontsize=10)
    fig.tight_layout(rect=(0,.13,1,1));pdf.savefig(fig);fig.savefig(P/'vues/implantation-M3.png',dpi=140);plt.close(fig)
    fig,axes=plt.subplots(2,1,figsize=(11.69,8.27));ax=axes[0]
    for x,w,label,color in [(0,2.4,'Cassette','#98adc0'),(4.7,3,'Bride rapprochée','#4fa99c'),(7.7,2.4,'Écrou M3','#777777')]:
        ax.add_patch(Rectangle((x,-2),w,4,color=color,alpha=.7));ax.text(x+w/2,2.7,label,ha='center',fontsize=10)
    ax.add_patch(Rectangle((0,-.35),12,.7,color='#343434'));ax.add_patch(Rectangle((-3.2,-1.5),3.2,3,color='#343434'))
    ax.text(10.8,-2.9,'Émergence nominale1,9mm',ha='center',fontsize=10);ax.set_xlim(-4,15);ax.set_ylim(-4,5);ax.grid(alpha=.2);ax.set_aspect('equal');ax.set_xlabel('Distance depuis sous-tête / mm');ax.set_title('Coupe de principe bride nappe — M3×12, sans rondelle ajoutée')
    ax=axes[1]
    ax.add_patch(Rectangle((-4,-1.6),8,1.6,color='#496b83'));ax.add_patch(Rectangle((-4,10),8,1.6,color='#649754'))
    for x in [-2.5,1.2]:ax.add_patch(Rectangle((x,0),1.3,10,color='#dfad55'))
    ax.add_patch(Rectangle((-1,-3),2,17.5,color='#555555'));ax.add_patch(Rectangle((-2.5,11.6),5,1.4,color='#dfad55'));ax.add_patch(Rectangle((-2,13),4,1.6,color='#777777'))
    ax.add_patch(Rectangle((-2.5,-3),5,1.4,color='#dfad55'));ax.add_patch(Rectangle((-1.9,-5),3.8,2,color='#555555'))
    ax.annotate('',(5,0),(5,10),arrowprops={'arrowstyle':'<->'});ax.text(5.4,5,'H à mesurer\nsans effort',va='center');ax.text(-9,4,'Appui isolant\nporteur',va='center');ax.text(7,13,'Un écrou M2\nordinaire',va='center');ax.text(7,-2,'Une vis M2\nlongueur selon H',va='center')
    ax.set_xlim(-10,15);ax.set_ylim(-6,17);ax.set_aspect('equal');ax.axis('off');ax.set_title('FRAM/SD — principe non libéré, dessin symbolique non coté en hauteur')
    fig.suptitle('Deux fonctions distinctes : structure M3 métal / maintien de module isolé')
    fig.tight_layout();pdf.savefig(fig);plt.close(fig)
print('Two-page PDF and assembly-axis PNG generated')
