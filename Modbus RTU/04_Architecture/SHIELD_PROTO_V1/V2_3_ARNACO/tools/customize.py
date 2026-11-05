"""Native KiCad Python: derive V23 from V19, adding only three F.SilkS texts."""
from pathlib import Path
import json
import uuid
import pcbnew as p

P = Path(__file__).resolve().parents[1]
source = P.parent / 'V1_9_C05/cad/TR2_Shield_Proto_V1.kicad_pcb'
dest = P / 'cad/TR2_Shield_Proto_V1.kicad_pcb'
assert source != dest and 'V2_3_ARNACO' in str(dest)
b = p.LoadBoard(str(source))
rows = []
for text, x, y, size, stroke in [
    ('ARNACO', 61.8, 88.0, 2.4, .25),
    ('©', 70.5, 86.8, 1.1, .16),
    ('Votre crédulité est notre force', 61.8, 92.0, 1.0, .15),
]:
    t = p.PCB_TEXT(b)
    t.SetText(text)
    t.SetLayer(p.F_SilkS)
    t.SetPosition(p.VECTOR2I(round(x * 1e6), round(y * 1e6)))
    t.SetTextSize(p.VECTOR2I(round(size * 1e6), round(size * 1e6)))
    t.SetTextThickness(round(stroke * 1e6))
    t.SetUuid(p.KIID(str(uuid.uuid5(uuid.NAMESPACE_URL, 'TR2-V23-' + text))))
    b.Add(t)
    bb = t.GetBoundingBox()
    rows.append({'text': text, 'uuid': t.m_Uuid.AsString(), 'position_mm': [x, y],
                 'size_mm': size, 'stroke_mm': stroke, 'layer': t.GetLayerName(),
                 'bbox_mm': [p.ToMM(bb.GetLeft()), p.ToMM(bb.GetTop()),
                             p.ToMM(bb.GetRight()), p.ToMM(bb.GetBottom())]})
p.SaveBoard(str(dest), b)
dest.write_text(dest.read_text(encoding='utf-8'), encoding='utf-8', newline='\n')
(P / 'preuves/textes-natifs.json').write_text(json.dumps(rows, indent=2, ensure_ascii=False) + '\n', encoding='utf-8', newline='\n')
print(json.dumps(rows, indent=2, ensure_ascii=False))
