from hardware import *
old=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
assert hashlib.sha256(old.with_suffix('.bin').read_bytes()).hexdigest()=='00ba4f5b6750f3c20b2bd18832c99eaf10ac257449df6df12a53f6500ed81e5d'
original=(P/'flash-before.bin').read_bytes();assert len(original)==2097152 and hashlib.sha256(original).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
current=(P/'fram-after-resume.bin').read_bytes();assert len(current)==262144
n=max((P/'r4m-resume-image.bin').stat().st_size,old.with_suffix('.bin').stat().st_size);n=(n+8191)//8192*8192
image=P/'restore-inherited-pages.bin';assert not image.exists();image.write_bytes(original[:n])
flash('restore-inherited-firmware',image,'fresh double-read Flash backup at11719e3; inherited M016 attempt02')
debug('restore-full-flash-reset',old,IDENTITY+f'dump binary memory {win(P/"flash-restored.bin")} 0x08000000 0x08200000\nmonitor reset\n')
assert (P/'flash-restored.bin').read_bytes()==original
time.sleep(2)
debug('restore-final-fram-state',old,IDENTITY+'''info registers pc sp xpsr
frame function CommissioningReadOnly
p storage
p tr2_commission_fram_result
set $r4m_final=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)
p $r4m_final
if $r4m_final != 0
 quit 4
end
'''+f'dump binary memory {win(P/"fram-after-restoration.bin")} tr2_commission_fram_backup tr2_commission_fram_backup+262144\nframe 0\n',timeout=60)
assert (P/'fram-after-restoration.bin').read_bytes()==current
before=(P/'fram-before.bin').read_bytes();assert current[:122880]==before[:122880] and current[245760:]==before[245760:]
r=dict(restoration='PASS',flash_full_2MiB_exact=True,flash_sha256=hashlib.sha256(original).hexdigest(),fram_after_restoration_unchanged=True,fram_sha256=hashlib.sha256(current).hexdigest(),historical_and_tail_unchanged=True,firmware='Inherited M016 attempt02 commissioning WAIT, exact initial Flash; no cumulative successor qualification',programmed_bytes=n,pages=n//8192,option_bytes_changed=False,mass_erase=False,fram_rewound=False,final_state='CommissioningReadOnly waiting; GDB detached',utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(P/'restoration-summary.json').write_text(json.dumps(r,indent=2)+'\n');print(r,flush=True)
