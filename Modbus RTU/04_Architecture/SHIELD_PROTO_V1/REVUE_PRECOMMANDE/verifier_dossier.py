"""Read-only release audit + new manifests/BOM, no CAD save/network/hardware/order.
Requires successful native KiCad/ERC/DRC/CAM proofs already captured in preuves/.
Does not turn an absent physical check or quote into GO.
"""
from pathlib import Path
import json,csv,hashlib,subprocess,zipfile,math
from decimal import Decimal as D
P=Path(__file__).resolve().parent;ROOT=Path(subprocess.check_output(['git','rev-parse','--show-toplevel'],cwd=P,text=True).strip());S=P.parent;Q=P/'preuves'
BASE='6d6417a86d7bdb2a725bd5557bbd4dcac413b7f8'
def read(path):return json.loads(path.read_text())
def save(path,data):path.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n')
def sha(b):return hashlib.sha256(b).hexdigest()

# Protect every existing shield artifact (including prior CAM and mechanical outputs).
frozen=[]
for row in subprocess.check_output(['git','ls-tree','-rz',BASE,'--',str(S.relative_to(ROOT))],cwd=ROOT).split(b'\0'):
 if not row:continue
 meta,n=row.split(b'\t');n=n.decode();blob=meta.split()[2].decode();b=(ROOT/n).read_bytes();assert b==subprocess.check_output(['git','cat-file','blob',blob],cwd=ROOT),n
 frozen.append({'path':n,'bytes':len(b),'sha256':sha(b)})
save(Q/'conservation.json',{'base':BASE,'files_count':len(frozen),'files':frozen,'scope':'every pre-existing shield tracked file byte-identical; no firmware/PCB/CAM legacy changes'})

erc=read(Q/'ERC.json');drc=read(Q/'DRC.json');assert not any(s.get('violations',[]) for s in erc['sheets'])
for field in ['violations','unconnected_items','schematic_parity']:assert not drc.get(field,[]),(field,drc.get(field))
native=read(Q/'native-geometry.json');assert native['board_thickness_mm']==1.6 and native['copper_layer_count']==2
verify=read(Q/'export-verification.json');assert verify['status']=='PASS' and verify['connected_pins']==59 and verify['pads']==191 and verify['vias']==4
b=read(Q/'pcb-readback.json');pads={(p['ref'],p['number']):p for p in b['pads'] if p['number']}
assert pads['J5','5']['net']=='RTU_DE'
nc=[pads['J5',str(n)]['net'] for n in [1,2,3,4,6]];assert len(set(nc))==5 and all(x.startswith('unconnected-') for x in nc)
assert read(Q/'drill-spacing.json')['PTH/PTH']['edge_gap_mm']>=.45
assert min(native['track_widths_mm'])>=.25 and not any('GND2' in n for n in native['netnames'])

# Check the two sourcing variants against real saved pads and existing footprint body envelopes.
# Floating subtraction gives20.319999999999993; tolerance = one1nm native grid unit.
assert abs(math.dist([pads['J6','1']['x'],pads['J6','1']['y']],[pads['J6','9']['x'],pads['J6','9']['y']])-20.32)<1e-6
assert abs(math.dist([pads['C2','1']['x'],pads['C2','1']['y']],[pads['C2','2']['x'],pads['C2','2']['y']])-2)<1e-6
variant={'Harwin_M20_7820946':{'source':'https://www.harwin.com/products/M20-7820946','drawing':'https://content.harwin.com/asset/53851ba8-b32e-4ac5-a0c5-09d9271c0c0a/DRG-00385-Technical-Drawing-Datasheet-M20-782-pdf.pdf','contacts':9,'pitch_mm':2.54,'height_mm':[8.35,8.65],'body_length_max_mm':23.61,'body_width_max_mm':2.65,'existing_reserved_body_length_width_mm':[24.16,3.29],'recommended_hole_mm':1.02,'saved_hole_mm':1.02,'status':'nominal PCB footprint fit; module engagement/plating choice NOT physically qualified; proposed purchasing option only'},'Panasonic_EEUFR1H100':{'source':'https://industrial.panasonic.com/ww/products/pt/aluminum-cap-lead/models/EEUFR1H100','capacitance_uF':10,'voltage_V':50,'pitch_mm':2.0,'body_diameter_mm':5.0,'body_length_mm':11,'saved_C2_body_envelope_diameter_mm':5.5,'saved_pitch_mm':2.0,'status':'value/polarity/nominal footprint compatible; proposed alternative to ECA1HM100, no assembly or lifetime qualification'}}
assert variant['Harwin_M20_7820946']['body_length_max_mm']<24.16 and variant['Harwin_M20_7820946']['body_width_max_mm']<3.29
save(Q/'variantes-compatibilite.json',variant)
save(Q/'audit-synthese.json',{'status':'PASS_ELECTRICAL_CAD_ONLY','ERC_violations':0,'DRC_violations':0,'unconnected':0,'schematic_parity_errors':0,'connected_pins':59,'all_pads':191,'NC_numbered_pads':121,'NPTH':11,'vias':4,'outline_mm':[90,100],'thickness_mm':1.6,'layers':2,'J5_pad5':'CN12-69/PG4=RTU_DE','J5_other5':'independent NC','physical_fit':'NOT_VERIFIED','fabrication_release':'NO_GO_PENDING_SINGLE_PHYSICAL_CHECK'})
if (Q/'supplement-KiCad.json').exists():
 supplement=read(Q/'supplement-KiCad.json');summary=read(Q/'audit-synthese.json')
 summary['initial_strict_vertex_probe']='FAIL_EXPLAINED_INTERMEDIATE_VERTEX_NOT_A_VIA_ENTRY' if supplement['track_ends_touching_via_not_centered'] else 'PASS'
 if (Q/'verification-entree-via.json').exists():
  entry=read(Q/'verification-entree-via.json');assert entry['status']=='PASS' and len(entry['vias'])==4;summary['actual_via_centered_entry_both_layers']='PASS'
 summary['all_footprints_have_courtyard']=not supplement['missing_courtyards'];summary['ERC_default_ignored_checks']=erc['ignored_checks'];summary['DRC_default_ignored_checks']=drc['ignored_checks'];save(Q/'audit-synthese.json',summary)

# Normalize ONLY new text exports, preserve tokens; old files remain untouched.
normal=[];cam=P/'CAM_CANDIDAT_NON_LIBERE'
for f in sorted(cam.iterdir()):
 b=f.read_bytes();text=b.decode('utf-8');new=('\n'.join(x.rstrip() for x in text.splitlines())+'\n').encode();assert b.split()==new.split();f.write_bytes(new)
 normal.append({'file':f.name,'raw_sha256':sha(b),'normalized_sha256':sha(new),'tokens_identical':True})
save(Q/'CAM-normalisation.json',normal)
manifest=[{'file':f.name,'bytes':f.stat().st_size,'sha256':sha(f.read_bytes())} for f in sorted(cam.iterdir())]
assert len(manifest)==10
save(P/'CAM-manifeste.json',{'source_head':BASE,'CAD_source_PCB_sha256':sha((S/'cad/TR2_Shield_Proto_V1.kicad_pcb').read_bytes()),'status':'NON_LIBERE','files':manifest})
zipname='TR2_SHIELD_V14_NON_LIBERE_V18.zip'
with zipfile.ZipFile(P/zipname,'w',compression=zipfile.ZIP_DEFLATED) as z:
 for f in sorted(cam.iterdir()):
  info=zipfile.ZipInfo(f.name,date_time=(2026,11,4,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;z.writestr(info,f.read_bytes())
with zipfile.ZipFile(P/zipname) as z:
 assert z.testzip() is None
 for f in manifest:assert sha(z.read(f['file']))==f['sha256']

a=read(P/'approvisionnement.json');rate=D(str(a['exchange']['EUR_USD']));vat=D('1.20');rows=[];budgets={}
for scenario in ['rapide_2_distributeurs','economique_3_distributeurs']:
 totals={}
 for item in a['items']:
  if item['scenario'] not in ['commun',scenario]:continue
  assert item['qty_buy']>=item['qty_need'] and item['qty_buy']>=item['MOQ'] and item['qty_buy']%item['multiple']==0
  v=D(str(item['unit_USD']))*item['qty_buy'];totals[item['supplier']]=totals.get(item['supplier'],D(0))+v
  euro=v/rate*vat
  rows.append({'scenario':scenario,'repere':item['ref'],'MPN':item['mpn'],'besoin_2':item['qty_need'],'achat':item['qty_buy'],'prix_unitaire_USD_HT_affiche':item['unit_USD'],'ligne_USD_HT':str(v),'ligne_EUR_TTC_convertie_INDICATIVE':str(round(euro,2)),'fournisseur':item['supplier'],'stock_affiche_non_reserve':item['stock_displayed'],'date_consultation':'2026-11-04','port_FR_TTC':'a mutualiser voir budget','delai_FR': 'TME2-4jouv apres expedition/paiement; pas date ferme' if item['supplier']=='TME' else ('DigiKey environ3jours apres expedition, dedouanement variable' if item['supplier']=='DigiKey' else 'NON VERIFIE'),'source':item['source'],'reserve':item['note']})
 tme=totals['TME']/rate*vat+D('7.08');dk_products=totals['DigiKey']/rate*vat
 low=tme+dk_products+D('25');high=tme+dk_products+D('30')
 if 'Tayda' in totals:low+=totals['Tayda']/rate*vat;high+=totals['Tayda']/rate*vat
 budgets[scenario]={'goods_USD_HT_by_supplier':{k:str(v) for k,v in totals.items()},'TME_goods_plus_economy_shipping_EUR_TTC_indicative':str(round(tme,2)),'DigiKey_goods_EUR_TTC_indicative':str(round(dk_products,2)),'subtotal_known_goods_plus_TME_and_DigiKey_shipping_EUR_TTC_INDICATIVE':[str(round(low,2)),str(round(high,2))],'excludes':['PCB renduTTC','fixations et entretoises','boitier/impression/PC/joints','Tayda port/import/frais si scenario3distributeurs','ecart prixFrance/banque/reemploi manquant'],'firm_delivered_total_EUR':None,'status':'BUDGET_CONVERTI_NON_DEVIS'}
 # Unpriced lines remain visible in the BOM, never silently treated as zero.
 for name,qty,mpn,desc,url in [('PCB',5,'LOT5_FR4_90x100_2L_1p6','2 montes +3 reserves; devis public fabrication5.30USD, portFrance7.33/16.11/23.44USD selon service; TTC final non verifie',a['PCB_options']['quote_URL'])]+[(x['ref'],x['qty_need'],x.get('candidate','SPEC_A_FIGER'),x['spec'],x.get('source','')) for x in a['other_costs']]:
  row={k:'' for k in rows[0]};row.update(scenario=scenario,repere=name,MPN=mpn,besoin_2=qty,achat=qty,fournisseur='A_CONFIRMER',stock_affiche_non_reserve='NON VERIFIE',date_consultation='2026-11-04',port_FR_TTC='NON VERIFIE',delai_FR='NON VERIFIE',source=url,reserve=desc);rows.append(row)
with (P/'BOM_2_PROTOTYPES.csv').open('w',newline='') as f:
 writer=csv.DictWriter(f,fieldnames=list(rows[0]),lineterminator='\n');writer.writeheader();writer.writerows(rows)
save(P/'budget-TTC.json',{'exchange':a['exchange'],'VAT_budget':.2,'scenarios':budgets,'five_PCB_total_delivered_EUR_TTC':None,'two_operational_shields_plus_five_PCB_EUR_TTC':None,'commercial_quote_complete':False,'public_PCB_quote':a['PCB_options'].get('public_estimate'),'reason':'public PCB fabrication and France freight estimated; final VAT/import invoice and mechanical line prices missing; no firm complete total or arrival date','zip_non_libere':zipname,'zip_sha256':sha((P/zipname).read_bytes())})
save(P/'CONFIGURATION_5PCB.json',a['PCB_options'])
print(json.dumps({'preserved':len(frozen),'CAM_files':len(manifest),'zip_sha256':sha((P/zipname).read_bytes()),'budget':budgets},ensure_ascii=False))
