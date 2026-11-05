"""Usage: PYTHONPATH=<olefile install> python extract_st.py <external ST extraction directory>.
Requires original BDP4.0 and manufacturing3.4 (hashes in PROVENANCE.md), native CFB parser olefile0.47.
Binary pad layout checked against KiCad10 official altium_parser_pcb.cpp APAD6, no full ST archive redistributed.
"""
from pathlib import Path
import olefile,struct,json,math,sys
p=Path(sys.argv[1]).resolve();o=olefile.OleFileIO(p/'ST-design/MB1549C/MB1549.PcbDoc')
def records(b):
 i=0;res=[]
 while i<len(b):
  n=struct.unpack_from('<I',b,i)[0];i+=4;s=b[i:i+n].decode('cp1252').strip('\0');i+=n;res.append(dict(t.split('=',1)for t in s.split('|')if'='in t))
 assert i==len(b);return res
board=records(o.openstream('Board6/Data').read())[0];components=records(o.openstream('Components6/Data').read());orig=[float(board[k].replace('mil',''))*.0254 for k in ['ORIGINX','ORIGINY']]
b=o.openstream('Pads6/Data').read();i=0;pads=[]
while i<len(b):
 assert b[i]==2,(i,b[i]);i+=1;subs=[]
 for z in range(6):
  n=struct.unpack_from('<I',b,i)[0];i+=4;subs.append(b[i:i+n]);i+=n
 name=subs[0][1:1+subs[0][0]].decode('cp1252');d=subs[4];assert len(d)>=110
 comp=struct.unpack_from('<H',d,7)[0];net=struct.unpack_from('<H',d,3)[0];xx,yy=struct.unpack_from('<ii',d,13);hole=struct.unpack_from('<i',d,45)[0]*.00000254
 pads.append({'ref':components[comp].get('SOURCEDESIGNATOR','') if comp!=65535 else '__board__','component_index':comp,'number':name,'xy_ST_mm':[round(xx*.00000254-orig[0],8),round(yy*.00000254-orig[1],8)],'drill_mm':round(hole,6),'plated':bool(d[60]),'angle':struct.unpack_from('<d',d,52)[0],'net_index':net})
assert i==len(b)
# independent ODB++ pin-coordinate check, never assume its 2020 headers are C05.
odb={};current=None
for line in (p/'ST-odb/odb/steps/pcb/layers/comp_+_top/components').read_text(encoding='cp1252').splitlines():
 t=line.split()
 if line.startswith('CMP '):current=t[6]
 if line.startswith('TOP '):odb[(current,t[-1].removeprefix(current+'-'))]=[float(t[2])*25.4,float(t[3])*25.4]
selected=[x for x in pads if x['ref']in ['CN7','CN8','CN9','CN10','CN11','CN12']]
diffs=[]
for x in selected:
 v=odb[x['ref'],x['number']];dist=math.dist(x['xy_ST_mm'],v);diffs.append(dist);assert dist<.00002,(x,v,dist)
result={'source':'ST official MB1549.PcbDoc in MB1549 BDP4.0, project Board_PCB_Revision=C/Board_Assembly_Revision=05','origin_ST_mm':orig,'units':'Altium int32 1e-4mil; 0.00000254mm/unit; x right/y up top view; subtract Board6 origin','native_total_pads':len(pads),'header_pads_compared_to_independent_ODB':len(diffs),'max_ODB_delta_mm':max(diffs),'headers':selected,'mechanical_pads':[x for x in pads if x['drill_mm']>=2.7],'components':[{'ref':x.get('SOURCEDESIGNATOR'),'pattern':x.get('PATTERN'),'xy_ST_mm':[round(float(x[k].replace('mil',''))*.0254-orig[j],8)for j,k in enumerate(['X','Y'])],'angle':float(x['ROTATION']),'height_mm':float(x.get('HEIGHT','0mil').replace('mil',''))*.0254}for x in components if x.get('SOURCEDESIGNATOR')in ['CN7','CN8','CN9','CN10','CN11','CN12','CN1','CN15','B1','B2','LD1','LD2','LD3']]}
(p/'ST-coordinates.json').write_text(json.dumps(result,indent=2)+'\n');print('PAD',len(pads),'headers',len(diffs),'max independent ODB delta',max(diffs));print('holes',result['mechanical_pads'])
