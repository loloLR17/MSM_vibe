from hardware import *
a=(P/'flash-before.bin').read_bytes();b=(P/'fram-after-phase1c.bin').read_bytes()
assert len(a)==2097152 and hashlib.sha256(a).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
assert b==(P/'fram-before.bin').read_bytes() and hashlib.sha256(b).hexdigest()=='14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9'
elf=P.parent/'probe-build-03/tr2-r4s-readonly-service-NON-PRODUCTION.elf'
debug('restore-stopped-check',elf,IDENTITY+'''p r4s_result
p owner.acquisition.fifo.stopped
p owner.acquisition.fifo.stop_error
if r4s_result.stage != 10 || owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped || owner.acquisition.fifo.stop_error
 quit 4
end
''')
old=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
flashed=[json.loads((P.parent/f).read_text())['bin_bytes'] for f in ['phase1-build.json','phase1b-build.json','phase1c-build.json']]
n=max(flashed+[old.with_suffix('.bin').stat().st_size]);n=(n+8191)//8192*8192
image=P/'restore-pages.bin';assert not image.exists();image.write_bytes(a[:n])
flash('restore-M016',image,'fresh verified full M016 backup c5bc; only three readonly probes were flashed')
debug('restore-full-readback',old,IDENTITY+f'dump binary memory {win(P/"flash-restored.bin")} 0x08000000 0x08200000\nmonitor reset\n')
assert (P/'flash-restored.bin').read_bytes()==a;time.sleep(2)
debug('restore-final-fram',old,IDENTITY+'''frame function CommissioningReadOnly
p tr2_commission_stage
set $r=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)
p $r
if $r != 0
 quit 5
end
'''+f'dump binary memory {win(P/"fram-after-restoration.bin")} tr2_commission_fram_backup tr2_commission_fram_backup+262144\nframe 0\n',timeout=60)
assert (P/'fram-after-restoration.bin').read_bytes()==b
r=dict(result='PASS',flash_full_bytes=len(a),flash_sha256=hashlib.sha256(a).hexdigest(),fram_full_bytes=len(b),fram_sha256=hashlib.sha256(b).hexdigest(),flash_exact_initial=True,fram_exact_initial=True,historical_and_tail_exact=True,pages=n//8192,bytes=n,fram_writes=0,R2_admissions=0,phase2_or_phase3_performed=False,final='M016 CommissioningReadOnly WAIT; sensor stopped; GDB detached',mass_erase=False,option_bytes_changed=False,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(P/'restoration-summary.json').write_text(json.dumps(r,indent=2)+'\n');print(r)
