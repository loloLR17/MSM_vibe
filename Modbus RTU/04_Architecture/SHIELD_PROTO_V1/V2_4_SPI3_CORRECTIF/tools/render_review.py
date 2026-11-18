"""Render native exported SVG routing views for review, never alter CAM."""
from pathlib import Path
import pymupdf as pdf
P=Path(__file__).resolve().parents[1]
for name in ('implantation','cuivre-dessus','cuivre-dessous-miroir'):
 with pdf.open(P/'exports'/(name+'.svg'))as d:
  assert len(d)==1;d[0].get_pixmap(matrix=pdf.Matrix(3,3)).save(P/'exports'/(name+'.png'))
print('Rendered3nativeSVG views, no CAM mutation')
