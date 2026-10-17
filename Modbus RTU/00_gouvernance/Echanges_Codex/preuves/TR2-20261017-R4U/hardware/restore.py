from hardware import *
local=P.parent;initial=(P/'flash-before.bin').read_bytes();fram=(P/'live05-fram.bin').read_bytes()
assert len(initial)==2097152 and hashlib.sha256(initial).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
assert len(fram)==262144
before=(P/'fram-before.bin').read_bytes();assert fram[:122880]==before[:122880] and fram[245760:]==before[245760:]
elf=local/'probe-build-04/tr2-r4u-live-runtime-NON-PRODUCTION.elf'
debug('restore-stopped-check',elf,IDENTITY+'''p r4u_result
p r4m_cs_closed
p g_serial.tx_busy
p g_serial.tx_fault
p owner.acquisition.fifo.stopped
p owner.acquisition.fifo.stop_error
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped || owner.acquisition.fifo.stop_error
 quit 4
end
''')
old=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
sizes=[d.with_suffix('.bin').stat().st_size for d in local.glob('probe-build-*/*.elf')]
n=(max(sizes+[old.with_suffix('.bin').stat().st_size])+8191)//8192*8192
image=P/'restore-pages.bin';assert not image.exists();image.write_bytes(initial[:n]);flash('restore-M016',image,'verified M016 full backup; Flash only; NO FRAM rollback')
debug('restore-full-readback',old,IDENTITY+f'dump binary memory {win(P/"flash-restored.bin")} 0x08000000 0x08200000\nmonitor reset\n')
assert (P/'flash-restored.bin').read_bytes()==initial;time.sleep(2)
debug('restore-final-fram',old,IDENTITY+'''frame function CommissioningReadOnly
p tr2_commission_stage
set $r=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)
p $r
if $r != 0
 quit 5
end
'''+f'dump binary memory {win(P/"fram-after-restoration.bin")} tr2_commission_fram_backup tr2_commission_fram_backup+262144\nframe 0\n',timeout=60)
assert (P/'fram-after-restoration.bin').read_bytes()==fram
v=dict(result='PASS',flash_full_bytes=len(initial),flash_sha256=hashlib.sha256(initial).hexdigest(),flash_exact_initial=True,fram_full_bytes=len(fram),fram_sha256=hashlib.sha256(fram).hexdigest(),fram_exact_last_physical=True,historical_and_tail_exact=True,pages=n//8192,bytes=n,final='M016 CommissioningReadOnly WAIT; sensor stopped; GDB detached; new epoch/run preserved',mass_erase=False,option_bytes_changed=False,FRAM_rollback=False,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(P/'restoration-summary.json').write_text(json.dumps(v,indent=2)+'\n');print(v)
