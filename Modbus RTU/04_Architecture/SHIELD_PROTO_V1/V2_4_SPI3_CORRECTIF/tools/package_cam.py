"""Package only the nine freshly exported CAM files; no fabrication release."""
from pathlib import Path
import hashlib,json,zipfile
P=Path(__file__).resolve().parents[1];M=P/'exports/CAM_CANDIDAT_NON_LIBERE'
files=sorted(f for f in M.iterdir()if f.suffix in('.gtl','.gbl','.gts','.gbs','.gto','.gbo','.gm1','.drl'));assert len(files)==9
assert sum(f.suffix=='.drl'for f in files)==2
assert json.loads((P/'preuves/M01-native.json').read_text())['verdict']=='PASS_NATIVE_M01_MECHANICAL_IDENTITY'
assert json.loads((P/'preuves/M01-STEP-V27.json').read_text())['verdict']=='PASS_FROZEN_V27_STEP_INTERFACE_NUMERICAL'
z=P/'exports/TR2_SHIELD_V24_SPI3_CANDIDAT_NON_LIBERE.zip';assert not z.exists()
sha=lambda b:hashlib.sha256(b).hexdigest()
with zipfile.ZipFile(z,'w',zipfile.ZIP_DEFLATED)as out:
 for f in files:
  info=zipfile.ZipInfo(f.name,(2026,11,18,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;out.writestr(info,f.read_bytes())
manifest={f.name:sha(f.read_bytes())for f in files}
with zipfile.ZipFile(z)as inp:
 assert inp.testzip()is None and {n:sha(inp.read(n))for n in inp.namelist()}==manifest
result={'zip':z.name,'zip_SHA256':sha(z.read_bytes()),'files':manifest,'release':'CANDIDAT_NON_LIBERE','required_next':'Independent full E01 copper audit and ChatGPT review, no upload/payment/order authorized','gbrjob_and_drill_maps':'Native informational files outside ZIP, no duplicated outline or mixed historical CAM'}
(P/'preuves/CAM-manifeste.json').write_text(json.dumps(result,indent=2)+'\n');z.with_suffix('.zip.sha256').write_text(result['zip_SHA256']+'  '+z.name+'\n');print(z.name,result['zip_SHA256'])
