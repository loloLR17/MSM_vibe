"""Generate the editable schematic from the declared candidate connection matrix.
No hardware access. Deterministic UUIDs retain ERC/netlist/PCB correspondence.
The checked-in .kicad_pro is intentionally preserved: do not reset DRC/ERC settings.
"""
from pathlib import Path
import json,uuid,argparse
NS=uuid.UUID('fe98677c-901a-438b-bd88-32a412dfc316')
def uid(s):return str(uuid.uuid5(NS,s))
def q(s):return json.dumps(str(s),ensure_ascii=False)
def effect(size=1.0,justify=''):
 return '(effects (font (size %s %s)) %s)'%(size,size,('(justify '+justify+')')if justify else'')
def pin_layout(c):
 n=c['n'];kind=c['kind'];dual=kind in('dual','dil','click');rows=n if dual else (1 if kind in('resistor','capacitor')else n)
 pins=[]
 if kind in('resistor','capacitor'):
  return [('1',-7.62,0,0),('2',7.62,0,180)]
 for r in range(rows):
  y=(rows-1)*1.27-r*2.54
  if dual:
   a,b=(2*r+1,2*r+2)if kind=='dual'else(r+1,2*rows-r)
   pins.extend([(str(a),-7.62,y,0),(str(b),7.62,y,180)])
  else:pins.append((str(r+1),-7.62,y,0))
 return pins

def main():
 p=Path(__file__).resolve().parents[1];d=json.loads((p/'design.json').read_text());root=d['root_uuid'];name='TR2_Shield_Proto_V1';lib=[];placed=[];wires=[];labels=[];nc=[]
 for c in d['components']:
  ref=c['ref'];sym=ref+'_'+c['value'];cx,cy=c['schematic_xy'];pins=pin_layout(c);top=max(x[2] for x in pins)+1.27;bottom=min(x[2]for x in pins)-1.27
  def prop(k,v,x,y,hide=False):return '(property %s %s (at %g %g 0) %s %s)'%(q(k),q(v),x,y,'(hide yes)'if hide else'',effect())
  defs=[prop('Reference','J'if ref[0]=='J'else ref[0],0,top+2.54),prop('Value',c['value'],0,bottom-2.54),prop('Footprint',c['footprint'],0,0,True),prop('Datasheet','',0,0,True)]
  body='(symbol %s (rectangle (start -5.08 %g) (end 5.08 %g) (stroke (width 0.254) (type default)) (fill (type background))))'%(q(sym+'_0_1'),top,bottom)
  pin_defs=[]
  for number,x,y,angle in pins:
   typ=c['pin_types'].get(number,'passive');net=c['nets'].get(number)
   pin_defs.append('(pin %s line (at %g %g %d) (length 2.54) (name %s %s) (number %s %s))'%(typ,x,y,angle,q(net or'NC'),effect(0.762),q(number),effect(0.762)))
   px,py=round(cx+x,4),round(cy-y,4)
   if net:
    ex=px-6.35 if x<0 else px+6.35
    wires.append('(wire (pts (xy %g %g) (xy %g %g)) (stroke (width 0) (type default)) (uuid %s))'%(px,py,ex,py,q(uid(ref+number+'wire'))))
    labels.append('(global_label %s (shape bidirectional) (at %g %g 0) %s (uuid %s))'%(q(net),ex,py,effect(0.95,'right bottom'if x<0 else'left bottom'),q(uid(ref+number+'label'))))
   else:nc.append('(no_connect (at %g %g) (uuid %s))'%(px,py,q(uid(ref+number+'nc'))))
  lib.append('(symbol %s (pin_names (offset 0.508) (hide yes)) (exclude_from_sim no) (in_bom yes) (on_board yes) %s %s (symbol %s %s))'%(q('TR2:'+sym),' '.join(defs),body,q(sym+'_1_1'),' '.join(pin_defs)))
  props=[prop('Reference',ref,cx,cy-top-3.81),prop('Value',c['value'],cx,cy+(-bottom)+3.81),prop('Footprint',c['footprint'],cx,cy,True),prop('Datasheet','',cx,cy,True)]
  inst='(instances (project %s (path %s (reference %s) (unit 1))))'%(q(name),q('/'+root),q(ref))
  placed.append('(symbol (lib_id %s) (at %g %g 0) (unit 1) (exclude_from_sim no) (in_bom yes) (on_board yes) (dnp no) (uuid %s) %s %s %s)'%(q('TR2:'+sym),cx,cy,q(c['uuid']),' '.join(props),' '.join('(pin %s (uuid %s))'%(q(n),q(uid(ref+n+'pin')))for n,*_ in pins),inst))
 text='(text %s (at 203.2 15.24 0) %s (uuid %s))'%(q('TR2 SHIELD V1.2 - DRAFT / FABRICATION INTERDITE AVANT LEVEE DES RESERVES'),effect(1.5),q(uid('warning')))
 s='(kicad_sch (version 20250610) (generator "eeschema") (uuid %s) (paper "A3") (title_block (title "TR2 shield prototype - interconnexion") (date "2026-11-03") (rev "1.2 DRAFT") (company "MSM TR2")) (lib_symbols %s) %s %s %s %s %s (embedded_fonts no))'%(q(root),'\n'.join(lib),text,'\n'.join(wires),'\n'.join(labels),'\n'.join(nc),'\n'.join(placed))
 (p/'cad/TR2.kicad_sym').write_text('(kicad_symbol_lib (version 20241209) (generator \"kicad_symbol_editor\") '+ '\n'.join(x.replace('(symbol \"TR2:', '(symbol \"',1)for x in lib)+')\n');(p/'cad/sym-lib-table').write_text('(sym_lib_table (version 7) (lib (name \"TR2\") (type \"KiCad\") (uri \"${KIPRJMOD}/TR2.kicad_sym\") (options \"\") (descr \"Declared shield interfaces\")))\n');out=p/'cad'/name;out.with_suffix('.kicad_sch').write_text(s+'\n');
 (p/'cad/fp-lib-table').write_text('(fp_lib_table (version 7) (lib (name "TR2") (type "KiCad") (uri "${KIPRJMOD}/TR2.pretty") (options "") (descr "Sourced prototype interfaces; see release gate")))\n');print(out.with_suffix('.kicad_sch'))
if __name__=='__main__':main()
