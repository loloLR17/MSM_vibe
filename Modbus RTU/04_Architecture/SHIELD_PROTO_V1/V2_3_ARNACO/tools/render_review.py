"""Render actual exported vectors for review, without touching fabrication data."""
from pathlib import Path
import json
import pymupdf as pdf

P = Path(__file__).resolve().parents[1]
files = []
for name, scale in [('ARNACO-CAM-detail', 1.2), ('serigraphie-depuis-Gerber', 1.2),
                    ('implantation', 4), ('cuivre-dessus', 4), ('cuivre-dessous-miroir', 4)]:
    with pdf.open(P / 'exports' / (name + '.svg')) as doc:
        assert len(doc) == 1
        out = P / 'exports' / (name + '.png')
        doc[0].get_pixmap(matrix=pdf.Matrix(scale, scale)).save(out)
        files.append(str(out.relative_to(P)))
(P / 'preuves/rendus.json').write_text(json.dumps({'rendered_files': files,
    'fabrication_data_changed': False, 'method': 'MuPDF opens exported SVG; CAM SVG comes from actual GTO strokes, not a font recreation'}, indent=2) + '\n')
print('Rendered', len(files), 'review images')
