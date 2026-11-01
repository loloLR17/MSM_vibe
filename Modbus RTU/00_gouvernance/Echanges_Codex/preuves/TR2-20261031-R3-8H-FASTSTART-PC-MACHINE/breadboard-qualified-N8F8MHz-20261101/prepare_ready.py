from pathlib import Path
import json,subprocess,sys,datetime
P=Path(__file__).parent;a=json.loads((P/'artifact.json').read_text());g=json.loads((P/'GATE_HOST_VERIFICATION.json').read_text());r=json.loads((P/'breadboard8-gate-collected/result.json').read_text());v=json.loads((P/'breadboard8-gate-collected/analysis.json').read_text());assert g['status']=='COMPLETE' and g['upstream_RAM_digest_exact'] and v['pass_references'] and v['numeric_prefix_verified_outputs']==103 and not r['flags'] and not r['rails'] and not r['clips']
ready=dict(prepared=True,long_never_started=True,code_sha=a['code_sha'],bin_sha256=a['images'][0]['sha256'],SPI3_Hz=8000000,prefix='N8F',short_file='N8F0003.TR2',long_file='N8F0005.TR2',gate_payload_bytes=g['payload_bytes'],gate_payload_sha256=g['sha256'],gate_file_sha256=g['file_sha256'],full_gate_copied_and_verified=True,checked_scope='two-second FIFO/FIR/SD gate, not endurance or SI metrology',sensor='borrowed TR2-001 module per user',requires_explicit_signal='LANCE 8H',preparation_preflight_complete=False)
f=P/'READY.json';assert not f.exists();f.write_text(json.dumps(ready,indent=2)+'\n')
try:
 with (P/'ready-preflight.log').open('wb') as log:subprocess.run([sys.executable,str(P/'launch_8h.py'),'--check-only'],stdout=log,stderr=subprocess.STDOUT,check=True,timeout=80)
except Exception:
 ready['prepared']=False;ready['preparation_preflight_complete']=False;f.write_text(json.dumps(ready,indent=2)+'\n');raise
ready['preparation_preflight_complete']=True;ready['prepared_at_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();f.write_text(json.dumps(ready,indent=2)+'\n');print(json.dumps(ready,indent=2))
