from hardware import *
assert json.loads((P/'physical-summary.json').read_text())['result']=='PASS'
a=(P/'flash-before.bin').read_bytes();b=(P/'fram-after-probe.bin').read_bytes()
assert len(a)==2097152 and hashlib.sha256(a).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
assert b==(P/'fram-before.bin').read_bytes()
old=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
n=max((P.parent/'probe-build/tr2-r4q-readonly-rtu-NON-PRODUCTION.bin').stat().st_size,old.with_suffix('.bin').stat().st_size);n=(n+8191)//8192*8192
image=P/'restore-pages.bin';assert not image.exists();image.write_bytes(a[:n])
flash('restore-initial-flash',image,'fresh full backup c5bc344d, verified identical to R4P initial M016')
debug('restore-full-readback',old,IDENTITY+f'dump binary memory {win(P/"flash-restored.bin")} 0x08000000 0x08200000\nmonitor reset\n')
assert (P/'flash-restored.bin').read_bytes()==a;time.sleep(2)
debug('restore-final-fram',old,IDENTITY+'''frame function CommissioningReadOnly
p storage
set $r=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)
p $r
if $r != 0
 quit 4
end
'''+f'dump binary memory {win(P/"fram-after-restoration.bin")} tr2_commission_fram_backup tr2_commission_fram_backup+262144\nframe 0\n',timeout=60)
assert (P/'fram-after-restoration.bin').read_bytes()==b
(P/'restoration-summary.json').write_text(json.dumps(dict(result='PASS',flash_full_2MiB_exact=True,flash_sha256=hashlib.sha256(a).hexdigest(),fram_full_unchanged=True,fram_sha256=hashlib.sha256(b).hexdigest(),historical_and_tail_equal=True,programmed_pages=n//8192,programmed_bytes=n,fram_writes=0,mass_erase=False,option_bytes_changed=False,final_firmware='M016 CommissioningReadOnly WAIT, sensor stopped, debugger detached',utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
print('RESTORATION_FULL_FLASH_AND_FRAM_PASS',n,flush=True)
