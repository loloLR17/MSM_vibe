from hardware import *
a=(P/'flash-before.bin').read_bytes();b=(P/'fram-after-live.bin').read_bytes();before=(P/'fram-before.bin').read_bytes()
assert len(a)==2097152 and hashlib.sha256(a).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
assert len(b)==262144 and b[:122880]==before[:122880] and b[245760:]==before[245760:]
elf=P.parent/'probe-build-02/tr2-r4r-physical-time-NON-PRODUCTION.elf'
debug('restore-stopped-check',elf,IDENTITY+'''p r4r_result.stage
p r4r_result.error
p owner.acquisition.fifo.stopped
p owner.acquisition.fifo.stop_error
p engine.indeterminate
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped || owner.acquisition.fifo.stop_error
 quit 4
end
''')
old=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
n=max((P.parent/'probe-build-02/tr2-r4r-physical-time-NON-PRODUCTION.bin').stat().st_size,old.with_suffix('.bin').stat().st_size);n=(n+8191)//8192*8192
image=P/'restore-pages.bin';assert not image.exists();image.write_bytes(a[:n])
flash('restore-initial-flash',image,'fresh full backup c5bc344d, exact M016 image; no FRAM rewind')
debug('restore-full-readback',old,IDENTITY+f'dump binary memory {win(P/"flash-restored.bin")} 0x08000000 0x08200000\nmonitor reset\n')
assert (P/'flash-restored.bin').read_bytes()==a;time.sleep(2)
debug('restore-final-fram',old,IDENTITY+'''frame function CommissioningReadOnly
p storage
p tr2_commission_stage
set $r=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)
p $r
if $r != 0
 quit 4
end
'''+f'dump binary memory {win(P/"fram-after-restoration.bin")} tr2_commission_fram_backup tr2_commission_fram_backup+262144\nframe 0\n',timeout=60)
assert (P/'fram-after-restoration.bin').read_bytes()==b
(P/'restoration-summary.json').write_text(json.dumps(dict(result='PASS',flash_full_2MiB_exact=True,flash_sha256=hashlib.sha256(a).hexdigest(),fram_after_live_preserved_exact=True,fram_sha256=hashlib.sha256(b).hexdigest(),historical_and_tail_equal=True,programmed_pages=n//8192,programmed_bytes=n,fram_writes_during_restoration=0,run5_never_reused_or_rolled_back=True,mass_erase=False,option_bytes_changed=False,final_firmware='M016 CommissioningReadOnly WAIT, sensor stopped, debugger detached',utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
print('RESTORATION_FULL_FLASH_AND_NEW_FRAM_PASS',n,hashlib.sha256(b).hexdigest(),flush=True)
