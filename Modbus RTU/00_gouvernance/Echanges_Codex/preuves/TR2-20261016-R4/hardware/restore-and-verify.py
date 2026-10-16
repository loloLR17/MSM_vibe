from hardware import *
old=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
oldbin=old.with_suffix('.bin');assert hashlib.sha256(oldbin.read_bytes()).hexdigest()=='00ba4f5b6750f3c20b2bd18832c99eaf10ac257449df6df12a53f6500ed81e5d'
image=P/'restore-inherited-pages-final.bin';assert image.read_bytes()==(P/'flash-before.bin').read_bytes()[:114688]
flash('restore-inherited-firmware',image,'backup verified at R4 initial HEAD 178f3481e087110ef3f897f02bc8077b4b018028; inherited M016 attempt02 artifact')
debug('restore-flash-full-readback-and-reset',old,IDENTITY+'dump binary memory '+win(P/'flash-restored.bin')+' 0x08000000 0x08200000\nmonitor reset\n')
assert (P/'flash-restored.bin').read_bytes()==(P/'flash-before.bin').read_bytes()
time.sleep(2)
debug('restore-final-fram-and-state',old,IDENTITY+'''info registers pc sp xpsr
frame function CommissioningReadOnly
p storage
p tr2_commission_fram_result
set $r4_final=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)
p $r4_final
if $r4_final != 0
 quit 4
end
'''+ 'dump binary memory '+win(P/'fram-after-restoration.bin')+' tr2_commission_fram_backup tr2_commission_fram_backup+262144\nframe 0\n')
assert (P/'fram-after-restoration.bin').read_bytes()==(P/'fram-after-r2-indeterminate.bin').read_bytes()
r=dict(restoration='PASS',flash_full_2MiB_exact=True,flash_sha256=hashlib.sha256((P/'flash-restored.bin').read_bytes()).hexdigest(),fram_after_restoration_unchanged=True,fram_sha256=hashlib.sha256((P/'fram-after-restoration.bin').read_bytes()).hexdigest(),historical_and_tail_unchanged=True,firmware='Inherited M016 attempt02 commissioning WAIT, exact initial Flash; not a newly qualified cumulative successor',programmed_bytes=114688,pages=14,option_bytes_changed=False,mass_erase=False,final_state='CommissioningReadOnly waiting; GDB detached',utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(P/'restoration-summary.json').write_text(json.dumps(r,indent=2)+'\n');print(r,flush=True)
