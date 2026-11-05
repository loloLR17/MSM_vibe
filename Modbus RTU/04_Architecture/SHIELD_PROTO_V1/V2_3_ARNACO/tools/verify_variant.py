"""Verify native-only text changes, CAM equality and actual GTO text placement.

The deliberately limited Gerber reader accepts this candidate's straight strokes
and circular flashes only; unsupported graphical commands fail instead of being
silently ignored. It is a review aid, not a general Gerber/DFM certification.
"""
from pathlib import Path
from collections import Counter
import hashlib
import json
import math
import re
import zipfile
import sexpdata as sx
from shapely.geometry import LineString, Point, box
from shapely.ops import unary_union

P = Path(__file__).resolve().parents[1]
R = P.parent
W = R.parents[2]
B = R / 'V1_9_C05'
M = P / 'exports/CAM_CANDIDAT_NON_LIBERE'


def write(name, value):
    (P / name).write_text(json.dumps(value, indent=2, ensure_ascii=False) + '\n')


def sha(data):
    return hashlib.sha256(data).hexdigest()


def normalize(text):
    # Only metadata timestamps, never coordinates, apertures or manufacturing data.
    prefixes = ('%TF.CreationDate,', 'G04 Created by ',
                '; #@! TF.CreationDate,', '; DRILL file KiCad ')
    return '\n'.join(l for l in text.splitlines() if not l.startswith(prefixes))


def silk_or_mask(file):
    lines = file.read_text().splitlines()
    assert '%FSLAX46Y46*%' in lines and '%MOMM*%' in lines
    apertures = {}
    current = None
    pos = None
    strokes, flashes = [], []
    for line in lines:
        if not line:
            continue
        a = re.fullmatch(r'%ADD(\d+)C,([0-9.]+)\*%', line)
        if a:
            apertures[a[1]] = float(a[2])
            continue
        a = re.fullmatch(r'D(\d+)\*', line)
        if a:
            current = apertures[a[1]]
            continue
        a = re.fullmatch(r'X(-?\d+)Y(-?\d+)D0([123])\*', line)
        if a:
            pt = (int(a[1]) / 1e6, -int(a[2]) / 1e6)
            assert current is not None
            if a[3] == '1':
                assert pos is not None
                strokes.append((pos, pt, current))
            elif a[3] == '3':
                flashes.append((pt, current))
            pos = pt
            continue
        if line.startswith(('G04 ', '%TF.', '%TO.', '%TA.')) or line in (
                '%FSLAX46Y46*%', '%MOMM*%', '%LPD*%', '%TD*%', 'G01*', 'M02*'):
            continue
        raise AssertionError(('Unsupported Gerber command', file.name, line))
    return strokes, flashes


source = sx.loads((B / 'cad/TR2_Shield_Proto_V1.kicad_pcb').read_text())
candidate = sx.loads((P / 'cad/TR2_Shield_Proto_V1.kicad_pcb').read_text())
words = {'ARNACO', '©', 'Votre crédulité est notre force'}
added = [n for n in candidate if isinstance(n, list) and str(n[0]) == 'gr_text' and n[1] in words]
assert len(added) == 3 and {n[1] for n in added} == words
assert source == [n for n in candidate if n not in added], 'Change outside added silkscreen text'
for n in added:
    assert next(x[1] for x in n if isinstance(x, list) and str(x[0]) == 'layer') == 'F.SilkS'
copies = []
for f in (B / 'cad').rglob('*'):
    if f.is_file() and f.suffix != '.kicad_pcb':
        assert f.read_bytes() == (P / 'cad' / f.relative_to(B / 'cad')).read_bytes(), f
        copies.append(str(f.relative_to(B)))
assert (B / 'design.json').read_bytes() == (P / 'design.json').read_bytes()
job = json.loads((M / 'TR2_Shield_Proto_V1-job.gbrjob').read_text())
assert job['GeneralSpecs']['LayerNumber'] == 2
assert job['GeneralSpecs']['BoardThickness'] == 1.6
assert job['GeneralSpecs']['Size'] == {'X': 90.05, 'Y': 100.05}
assert job['GeneralSpecs']['Finish'] == 'None'
assert [x['Thickness'] for x in job['MaterialStackup'] if x['Type'] == 'Copper'] == [.035, .035]
assert [x['Material'] for x in job['MaterialStackup'] if x['Type'] == 'Dielectric'] == ['FR4']

equivalences = []
for f in sorted(M.iterdir()):
    if f.suffix in ['.gtl', '.gbl', '.gts', '.gbs', '.gbo', '.gm1', '.drl']:
        old = B / 'exports/BLOCKED_FABRICATION' / f.name
        assert normalize(old.read_text()) == normalize(f.read_text()), f.name
        equivalences.append({'name': f.name, 'numeric_and_plot_commands_equal': True,
                             'source_SHA256': sha(old.read_bytes()), 'candidate_SHA256': sha(f.read_bytes()),
                             'normalized_SHA256': sha(normalize(f.read_text()).encode())})
assert len(equivalences) == 8
write('preuves/CAM-equivalence.json', {'normalization': 'line endings + creation timestamps only, no geometry removed', 'files': equivalences})

old_strokes, old_flashes = silk_or_mask(B / 'exports/BLOCKED_FABRICATION/TR2_Shield_Proto_V1-F_Silkscreen.gto')
strokes, flashes = silk_or_mask(M / 'TR2_Shield_Proto_V1-F_Silkscreen.gto')
assert not old_flashes and not flashes
lost = Counter(old_strokes) - Counter(strokes)
assert not lost, 'An old silkscreen stroke disappeared'
new = list((Counter(strokes) - Counter(old_strokes)).elements())
assert new
text_rows = json.loads((P / 'preuves/textes-natifs.json').read_text())
bounds = unary_union([box(*r['bbox_mm']) for r in text_rows])
new_ink = unary_union([LineString((a, b)).buffer(d / 2, quad_segs=24) for a, b, d in new])
assert bounds.buffer(.001).covers(new_ink), 'Unexpected silkscreen outside the three texts'
mask_strokes, mask_flashes = silk_or_mask(M / 'TR2_Shield_Proto_V1-F_Mask.gts')
assert not mask_strokes
mask = unary_union([Point(pt).buffer(d / 2, quad_segs=64) for pt, d in mask_flashes])
clearance = new_ink.distance(mask)
assert clearance > .3, ('Branding to soldermask opening clearance', clearance)
all_front_ink = unary_union([LineString((a, b)).buffer(d / 2, quad_segs=24) for a, b, d in strokes])
all_front_clearance = all_front_ink.distance(mask)
assert all_front_clearance >= .15, ('Front silkscreen to mask', all_front_clearance)
# Conservative existing module envelopes from native F.Fab readback; 10-way
# substitution bounding test adds2.54mm in ALL directions, not a fit guarantee.
module_boxes = {'FRAM_plus_one_pitch': [42.385-2.54, 56.385-2.54, 67.935+2.54, 74.315+2.54],
                'SD_plus_one_pitch': [20.105-2.54, 83.065-2.54, 43.115+2.54, 108.615+2.54],
                'Click_Fab': [83.335, 78.655, 140.635, 104.205],
                'MKI_Fab': [42.385, 21.385, 62.855, 54.555]}
module_clearances = {name: new_ink.distance(box(*v)) for name, v in module_boxes.items()}
assert all(x > 0 for x in module_clearances.values())
brand = next(r for r in text_rows if r['text'] == 'ARNACO')
copyright = next(r for r in text_rows if r['text'] == '©')
assert copyright['size_mm'] < brand['size_mm']
assert copyright['position_mm'][1] < brand['position_mm'][1]
assert copyright['bbox_mm'][0] > brand['bbox_mm'][2]
write('preuves/serigraphie-CAM.json', {
    'input': str((M / 'TR2_Shield_Proto_V1-F_Silkscreen.gto').relative_to(P)),
    'input_SHA256': sha((M / 'TR2_Shield_Proto_V1-F_Silkscreen.gto').read_bytes()),
    'old_strokes': len(old_strokes), 'new_total_strokes': len(strokes),
    'added_strokes': len(new), 'old_strokes_lost': len(lost),
    'added_ink_bbox_mm': list(new_ink.bounds), 'minimum_soldermask_opening_gap_mm': clearance,
    'whole_front_silk_to_mask_gap_mm': all_front_clearance,
    'module_envelope_clearance_mm': module_clearances, 'text_records': text_rows,
    'superscript_geometry': '© smaller, raised, right of ARNACO; native glyph, not literal (C)',
    'limits': 'Exact CAM stroke geometry checked; printing quality, physical modules and enclosure not qualified',
})

# Render actual GTO strokes as SVG, no text font or substitution involved here.
paths = ''.join(f'<path d="M{a[0]} {a[1]} L{b[0]} {b[1]}" stroke-width="{d}"/>' for a, b, d in strokes)
svg = f'<svg xmlns="http://www.w3.org/2000/svg" width="900" height="1000" viewBox="20 17 90 100"><rect x="20" y="17" width="90" height="100" fill="#166447"/><g fill="none" stroke="#fff" stroke-linecap="round" stroke-linejoin="round">{paths}</g></svg>'
(P / 'exports/serigraphie-depuis-Gerber.svg').write_text(svg)
zoom = svg.replace('width="900" height="1000" viewBox="20 17 90 100"', 'width="1200" height="480" viewBox="48 84 28 11.2"')
(P / 'exports/ARNACO-CAM-detail.svg').write_text(zoom)

erc = json.loads((P / 'docs/checks/ERC.json').read_text())
drc = json.loads((P / 'docs/checks/DRC.json').read_text())
assert not drc['violations'] and not drc['unconnected_items'] and not drc['schematic_parity']
assert all(not q['violations'] for q in erc['sheets'])
assert drc['ignored_checks'] == json.loads((B / 'docs/checks/DRC.json').read_text())['ignored_checks']
assert erc['ignored_checks'] == json.loads((B / 'docs/checks/ERC.json').read_text())['ignored_checks']
old = json.loads((P / 'preuves/conservation-initiale.json').read_text())
for n, h in old['files'].items():
    assert sha((W / n).read_bytes()) == h, n

cam_files = sorted(f for f in M.iterdir() if f.suffix in ['.gtl', '.gbl', '.gts', '.gbs', '.gto', '.gbo', '.gm1', '.drl'])
assert len(cam_files) == 9
manifest = {f.name: sha(f.read_bytes()) for f in cam_files}
zip_path = P / 'exports/TR2_SHIELD_V23_5PCB_CANDIDAT_NON_LIBERE.zip'
with zipfile.ZipFile(zip_path, 'w', compression=zipfile.ZIP_DEFLATED) as z:
    for f in cam_files:
        info = zipfile.ZipInfo(f.name, date_time=(2026, 11, 5, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        z.writestr(info, f.read_bytes())
with zipfile.ZipFile(zip_path) as z:
    assert z.testzip() is None
    assert {n: sha(z.read(n)) for n in z.namelist()} == manifest
write('preuves/CAM-manifeste.json', {'files': manifest, 'zip': zip_path.name,
                                  'zip_SHA256': sha(zip_path.read_bytes()), 'release': 'HUMAN_DECISION_PENDING',
                                  'gbrjob_excluded_from_ZIP': 'native informational job retained separately; size is stroke envelope90.05x100.05, Finish None is not the order setting',
                                  'drill_maps': 'two generated PDFs available separately, no second outline added to ZIP'})
write('preuves/validation-finale.json', {
    'status': 'PASS_NATIVE_AND_EXPORTS_ONLY', 'old_shield_files_unchanged': len(old['files']),
    'native_change_only_three_F_SilkS_texts': True, 'unchanged_native_copies': copies,
    'ERC_violations': 0, 'DRC_violations': 0, 'unconnected': 0, 'schematic_parity': 0,
    'project_rules_and_ignored_checks_unchanged': True, 'CAM_layers_and_drills_equivalent': len(equivalences),
    'old_silk_strokes_lost': 0, 'added_silk_strokes': len(new),
    'native_superscript_and_exact_accented_motto': True,
    'front_mask_clearance_mm': clearance, 'whole_front_silk_to_mask_gap_mm': all_front_clearance,
    'CAM_zip_files': len(cam_files),
    'physical_validation': False, 'manufacturing_release': 'HUMAN_DECISION_PENDING',
    'stack_nominal': '2 layers, FR4,1.6mm,2x35um; actual finish to select as lead-free HASL at future order',
    'gbrjob_size_warning': '90.05x100.05 stroke envelope; actual contour centerlines90x100 unchanged',
})
print(json.dumps({'PASS': True, 'old_files_preserved': len(old['files']), 'added_native_texts': 3,
                  'ERC': 0, 'DRC': 0, 'CAM_equivalent_files': len(equivalences),
                  'front_mask_clearance_mm': clearance, 'zip_SHA256': sha(zip_path.read_bytes())}, indent=2))
