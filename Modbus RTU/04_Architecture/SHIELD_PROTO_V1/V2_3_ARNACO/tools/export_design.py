"""Export this silkscreen candidate with the installed native KiCad CLI."""
from pathlib import Path
import json
import subprocess

P = Path(__file__).resolve().parents[1]
E, Q = P / 'exports', P / 'docs/checks'
M = E / 'CAM_CANDIDAT_NON_LIBERE'
M.mkdir(exist_ok=True)
exe = '/mnt/c/Program Files/KiCad/10.0/bin/kicad-cli.exe'


def win(path):
    return subprocess.check_output(['wslpath', '-w', str(path)], text=True).strip()


commands = []


def run(name, args):
    cmd = [exe] + [win(x) if isinstance(x, Path) else x for x in args]
    r = subprocess.run(cmd, capture_output=True, text=True)
    (Q / (name + '.log')).write_text(r.stdout + r.stderr)
    commands.append({'name': name, 'args': [str(x) for x in args], 'returncode': r.returncode})
    (Q / 'commands.json').write_text(json.dumps(commands, indent=2) + '\n')
    print(name, r.returncode, flush=True)
    r.check_returncode()


B = P / 'cad/TR2_Shield_Proto_V1.kicad_pcb'
S = B.with_suffix('.kicad_sch')
run('ERC', ['sch', 'erc', '--format', 'json', '--severity-all', '--exit-code-violations', '-o', Q / 'ERC.json', S])
run('DRC', ['pcb', 'drc', '--schematic-parity', '--all-track-errors', '--format', 'json', '--severity-all', '--exit-code-violations', '-o', Q / 'DRC.json', B])
run('schema', ['sch', 'export', 'pdf', '-o', E / 'schema.pdf', S])
run('netlist', ['sch', 'export', 'netlist', '--format', 'kicadxml', '-o', E / 'netlist.xml', S])
run('gerbers', ['pcb', 'export', 'gerbers', '--layers', 'F.Cu,B.Cu,F.Mask,B.Mask,F.Silkscreen,B.Silkscreen,Edge.Cuts', '--check-zones', '-o', M, B])
run('drill', ['pcb', 'export', 'drill', '--format', 'excellon', '--excellon-separate-th', '--generate-map', '--generate-report', '-o', M, B])
for name, layers, mirrored in [
    ('implantation-dessus', 'F.Fab,F.Silkscreen,Edge.Cuts', False),
    ('cuivre-dessus', 'F.Cu,F.Silkscreen,Edge.Cuts', False),
    ('cuivre-dessous-miroir', 'B.Cu,B.Silkscreen,Edge.Cuts', True),
]:
    run(name, ['pcb', 'export', 'pdf', '--layers', layers, '--mode-single'] +
        (['--mirror'] if mirrored else []) + ['-o', E / (name + '.pdf'), B])
for name, layers, mirrored in [('implantation', 'F.Fab,F.Silkscreen,Edge.Cuts', False),
                               ('cuivre-dessus', 'F.Cu,F.Silkscreen,Edge.Cuts', False),
                               ('cuivre-dessous-miroir', 'B.Cu,B.Silkscreen,Edge.Cuts', True)]:
    run(name + '-svg', ['pcb', 'export', 'svg', '--layers', layers, '--mode-single',
                        '--page-size-mode', '2', '--exclude-drawing-sheet',
                        '--black-and-white'] + (['--mirror'] if mirrored else []) +
                        ['-o', E / (name + '.svg'), B])
# Canonical LF only; no geometry or metadata fields changed.
for f in E.rglob('*'):
    if f.is_file() and f.suffix in ['.svg', '.xml', '.gbrjob', '.gtl', '.gbl', '.gts', '.gbs', '.gto', '.gbo', '.gm1', '.drl', '.rpt']:
        text = f.read_text()
        if f.suffix == '.svg':
            trimmed = '\n'.join(line.rstrip() for line in text.splitlines()) + '\n'
            assert text.split() == trimmed.split()
            text = trimmed
        f.write_text(text)
for f in Q.glob('*.json'):
    f.write_text(f.read_text())
