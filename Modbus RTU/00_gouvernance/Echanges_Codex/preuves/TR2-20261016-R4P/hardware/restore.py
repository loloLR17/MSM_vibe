from hardware import *
old=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
a=(P/'flash-before.bin').read_bytes();assert len(a)==2097152 and hashlib.sha256(a).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
b=(P/'fram-after-physical.bin').read_bytes();assert len(b)==262144
original_fram=(P/'fram-before.bin').read_bytes();assert original_fram[:122880]==b[:122880] and original_fram[245760:]==b[245760:]
base=Path('/home/lolo/dev/msm/reports/TR2-r4p')
n=max((base/'bench-build/tr2-r4p-coexistence-NON-PRODUCTION.bin').stat().st_size,old.with_suffix('.bin').stat().st_size);n=(n+8191)//8192*8192
image=P/'restore-pages.bin';assert not image.exists();image.write_bytes(a[:n])
flash('restore-inherited-firmware',image,'fresh verified Flash backup at03309c0; inherited M016 attempt02')
debug('restore-full-flash-reset',old,IDENTITY+f'dump binary memory {win(P/"flash-restored.bin")} 0x08000000 0x08200000\nmonitor reset\n')
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
r=dict(result='PASS',flash_full_2MiB_exact=True,flash_sha256=hashlib.sha256(a).hexdigest(),fram_unchanged_during_restoration=True,historical_and_tail_equal=True,fram_sha256=hashlib.sha256(b).hexdigest(),programmed_bytes=n,pages=n//8192,fram_restoration_writes=0,option_bytes_changed=False,mass_erase=False,final_state='Inherited M016 CommissioningReadOnly waiting, debugger detached; no cumulative LIVE firmware',utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(P/'restoration-summary.json').write_text(json.dumps(r,indent=2)+'\n');print(r)
