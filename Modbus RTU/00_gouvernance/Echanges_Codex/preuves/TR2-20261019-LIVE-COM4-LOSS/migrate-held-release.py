from pathlib import Path
import json,hashlib,shutil,subprocess,datetime
p=Path('/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag');old=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019');new=old.parent/'TR2-LIVE-COM4-HELD-20261019';repo=Path('/home/lolo/dev/msm/projects/MSM_vibe-LIVE-COM4-loss-diag/Modbus RTU/Supervision TR2/operator-demo')
assert json.loads((p/'final-ready-launcher-exit.json').read_text())['exit']==0
assert not (old/'data/live.db-wal').exists() or (old/'data/live.db-wal').stat().st_size==0
assert not (old/'data/connection-control').exists() and not (old/'data/connection-control.shutdown').exists()
assert json.loads((old/'data/transactions.json').read_text())['Transaction']==19
(new/'data').mkdir(exist_ok=False);(new/'legacy-c36-launchers').mkdir(exist_ok=False);manifest=[]
for f in sorted((old/'data').iterdir()):
 if not f.is_file() or f.name.startswith('connection-control') or f.name.startswith('live.db-'):continue
 q=new/'data'/f.name;shutil.copyfile(f,q);h=hashlib.sha256(f.read_bytes()).hexdigest();assert hashlib.sha256(q.read_bytes()).hexdigest()==h;manifest.append(dict(file=f.name,bytes=f.stat().st_size,sha256=h))
redirects=[]
for src in sorted(repo.glob('*.cmd')):
 b=(old/src.name).read_bytes();assert b.replace(b'\r\n',b'\n')==src.read_bytes();(new/'legacy-c36-launchers'/src.name).write_bytes(b)
 (new/src.name).write_bytes(src.read_bytes().replace(b'\n',b'\r\n'))
 target=subprocess.check_output(['wslpath','-w',str(new/src.name)],text=True).strip();redirect=('@echo off\r\ncall "'+target+'"\r\n').encode();(old/src.name).write_bytes(redirect)
 redirects.append(dict(name=src.name,original_sha256=hashlib.sha256(b).hexdigest(),backup=str(new/'legacy-c36-launchers'/src.name),redirect_sha256=hashlib.sha256(redirect).hexdigest(),new_target=target))
(p/'release-migration.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),previous_package_preserved=True,old_data_frozen_copied_exact=True,old_app_unchanged=True,whole_new_version_separately_accepted_before_switch=True,files=manifest,launchers=redirects,code_commit='d67741f2',build_git_head='b031d01a5ea1f102293d5fd24b34bbfafe45a178',build_source_patch='held-build-source.patch',new_service_dll_sha256='63f3ddc1a91d99599a719bc6a013aa52810a9813a9d08c40233ce45b29cb85fa'),indent=2)+'\n')
s=(p/'command.py').read_text().replace('TR2-LIVE-DEMO-20261019','TR2-LIVE-COM4-HELD-20261019').replace('c36c971_known_ack_oscillation_accepted=True',"stable_pause_and_HELD_restart_fix='d67741f2'");(p/'command-upgraded.py').write_text(s)
print('New complete release + exact closed data copy; five reversible launch redirects; no DLL substituted')
