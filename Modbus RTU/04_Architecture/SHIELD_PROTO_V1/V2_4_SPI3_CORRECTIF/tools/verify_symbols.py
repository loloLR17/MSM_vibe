"""Cross-check all 180 external-library versus embedded symbol pins and exports.
Directions for connected signals come from fresh manufacturer reference policy;
unconnected pins deliberately do not become electrically active by their alias.
"""
from pathlib import Path
import json,xml.etree.ElementTree as ET
import sexpdata as sx
P=Path(__file__).resolve().parents[1];T=lambda n,t:isinstance(n,list)and bool(n)and str(n[0])==t
one=lambda n,t:next(x for x in n if T(x,t))
lib=sx.loads((P/'cad/TR2.kicad_sym').read_text());sch=sx.loads((P/'cad/TR2_Shield_Proto_V1.kicad_sch').read_text());embedded=one(sch,'lib_symbols')
libraries={n[1]:n for n in lib if T(n,'symbol')};embedded={n[1]:n for n in embedded if T(n,'symbol')}

def pins(root):
 found={}
 def walk(n):
  if not isinstance(n,list):return
  if T(n,'pin')and isinstance(n[1],sx.Symbol):
   num=str(one(n,'number')[1]);found[num]=dict(name=one(n,'name')[1],type=str(n[1]),geometry=sx.dumps([x for x in n if not T(x,'name')and not T(x,'number')]))
  else:
   for x in n:walk(x)
 walk(root);return found

reference=json.loads((P/'preuves/E01/reference-constructeurs.json').read_text());by={(r['ref'],r['pin']):r for r in reference['rows']}
xml=ET.parse(P/'exports/netlist.xml').getroot();exported={(n.attrib['ref'],n.attrib['pin']):n.attrib for net in xml.findall('./nets/net')for n in net.findall('node')}
checked=[]
for instance in sch:
 if not T(instance,'symbol'):continue
 ref=next(n[2]for n in instance if T(n,'property')and n[1]=='Reference');ident=one(instance,'lib_id')[1]
 a,b=pins(libraries[ident.split(':')[1]]),pins(embedded[ident]);assert a==b,(ref,'external/embedded disagreement')
 for number,actual in a.items():
  key=ref,number;r=by[key];ex=exported[key]
  # Observed KiCad10 XML format appends _<physical pin> to pinfunction.
  xml_name=actual['name']+'_'+number
  xml_type=actual['type']+('+no_connect' if r['desired_net']=='NC' else '')
  assert xml_name==ex['pinfunction'] and xml_type==ex['pintype'], (key,actual,ex,xml_name,xml_type)
  want=r['desired_net'];direction=r['direction'];expected=None
  if want!='NC':
   if direction in('MCU output','module output'):expected='output'
   elif direction in('MCU input','module input'):expected='input'
   elif direction=='passive':expected='passive'
   if expected:assert actual['type']==expected,(ref,number,direction,actual)
  if ref=='J8'and int(number)in(19,20,21,22,23,24):
   expected_name={19:'CS',20:'SCL',21:'SDA',22:'SDO',23:'NC',24:'NC'}[int(number)];assert actual['name']==expected_name
  checked.append(dict(ref=ref,pin=number,vendor_function=r['vendor_function'],desired_net=want,symbol_name=actual['name'],symbol_type=actual['type'],connected_direction_required=expected,status='PASS'))
assert len(checked)==180
(P/'preuves/E01/symboles-types-180.json').write_text(json.dumps({'status':'PASS','pins_compared':len(checked),'rows':checked,'NC_policy':'Deliberately unused input/output/auxiliary pins may have a passive NC alias; vendor function remains explicit in independent reference. This is not a claim that the physical vendor pad is internally NC.'},indent=2,ensure_ascii=False)+'\n');print('PASS:180 external/embedded/export pins, connected signal directions and J8 manufacturer names')
