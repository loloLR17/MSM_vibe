from pathlib import Path
import hashlib,json,datetime,subprocess as s
p=Path('/home/lolo/dev/msm/reports/TR2-r4q');h=p/'hardware';root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4Q-live-b3-rtu')
assert (h/'flash-before.bin').read_bytes()==(h/'flash-restored.bin').read_bytes()
f=(h/'fram-before.bin').read_bytes();assert len(f)==262144
assert hashlib.sha256(f).hexdigest()=='a13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9'
assert f==(h/'fram-after-probe.bin').read_bytes()==(h/'fram-after-restoration.bin').read_bytes()
for n in ['physical-summary.json','restoration-summary.json']:assert json.loads((h/n).read_text())['result']=='PASS'
assert '100% tests passed, 0 tests failed out of 140' in (p/'full-validation.log').read_text()
assert '100% tests passed, 0 tests failed out of 1' in (p/'sanitize-window-test.log').read_text()
assert 'Four N=4096/8192/16384/32768' in (p/'window-test.log').read_text()
a=json.loads((p/'fram-audit.json').read_text());assert a['active_root']['generation']==37
for rec in a['journal']:
 selected=max(rec['copies'],key=lambda x:x['generation']);assert selected['phase']==3 and selected['status']==4 and selected['result']==0
assert max(a['epoch'],key=lambda x:x['generation'])['current']==4
assert max(a['intent_or_effect'],key=lambda x:x['generation'])['run_id']==4
code=s.check_output(['git','rev-parse','6e81350'],cwd=root,text=True).strip()
changes=s.check_output(['git','diff','--name-only',code],cwd=root,text=True).splitlines()
assert all('/00_gouvernance/' in n or n.endswith('/LIVE_R4Q.md') for n in changes),changes
r=dict(result='PASS',scope='software integration, host model, read-only MCU loopback, exact firmware restoration; no FIFO+B3 hardware or electrical RTU qualification',code_sha=code,full_fram_unchanged=True,flash_2MiB_restored=True,root_generation=37,epoch=4,run_id=4,no_new_indeterminate=True,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(p/'final-audit.json').write_text(json.dumps(r,indent=2)+'\n');print(r)
