"""Run real KiCad checks and create review exports. Does not authorize manufacture.
Usage: python3 export_design.py [kicad-cli path]. Windows CLI is supported from WSL.
"""
from pathlib import Path
import subprocess,json,sys,shutil
P=Path(__file__).resolve().parents[1];E=P/'exports';E.mkdir(exist_ok=True);M=E/'BLOCKED_FABRICATION';M.mkdir(exist_ok=True);Q=P/'docs/checks';Q.mkdir(exist_ok=True)
exe=sys.argv[1]if len(sys.argv)>1 else shutil.which('kicad-cli')or'/mnt/c/Program Files/KiCad/10.0/bin/kicad-cli.exe'
def path(p):return subprocess.check_output(['wslpath','-w',str(p)],text=True).strip()if exe.endswith('.exe')else str(p)
log=[]
def run(name,args,mandatory=True):
 a=subprocess.run([exe]+[path(x)if isinstance(x,Path)else x for x in args],capture_output=True,text=True);(Q/(name+'.log')).write_text(a.stdout+a.stderr);log.append({'name':name,'args':[str(x)for x in args],'returncode':a.returncode});(Q/'commands.json').write_text(json.dumps(log,indent=2)+'\n');print(name,a.returncode,flush=True)
 if mandatory:a.check_returncode()
B=P/'cad/TR2_Shield_Proto_V1.kicad_pcb';S=B.with_suffix('.kicad_sch')
run('ERC',['sch','erc','--format','json','--severity-all','--exit-code-violations','-o',Q/'ERC.json',S])
run('DRC',['pcb','drc','--schematic-parity','--all-track-errors','--format','json','--severity-all','--exit-code-violations','-o',Q/'DRC.json',B])
run('schematic-pdf',['sch','export','pdf','-o',E/'schema.pdf',S])
run('netlist',['sch','export','netlist','--format','kicadxml','-o',E/'netlist.xml',S])
run('gerbers',['pcb','export','gerbers','--layers','F.Cu,B.Cu,F.Mask,B.Mask,F.Silkscreen,B.Silkscreen,Edge.Cuts','--check-zones','-o',M,B])
run('drill',['pcb','export','drill','--format','excellon','--excellon-separate-th','--generate-map','--generate-report','-o',M,B])
run('assembly-pdf',['pcb','export','pdf','--layers','F.Fab,F.Silkscreen,Edge.Cuts','--sketch-pads-on-fab-layers','--mode-single','-o',E/'implantation-dessus.pdf',B])
run('bottom-assembly-pdf',['pcb','export','pdf','--layers','B.Fab,B.CrtYd,B.Silkscreen,Edge.Cuts','--sketch-pads-on-fab-layers','--mirror','--mode-single','-o',E/'implantation-dessous.pdf',B])
run('bottom-pdf',['pcb','export','pdf','--layers','B.Cu,B.Silkscreen,Edge.Cuts','--mirror','--mode-single','-o',E/'cuivre-dessous-miroir.pdf',B])
run('step-board-only',['pcb','export','step','--force','--board-only','-o',E/'shield-nu.step',B],mandatory=False)
run('3d-board-only',['pcb','render','--width','1400','--height','1100','--background','opaque','--rotate','-30,0,25','-o',E/'shield-nu.png',B],mandatory=False)
print('Exports are BLOCKED fabrication pending documented mechanical checks.')
