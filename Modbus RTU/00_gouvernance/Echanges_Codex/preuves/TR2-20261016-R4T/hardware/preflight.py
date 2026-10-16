from hardware import *
old=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
debug('flash-fresh-check',old,IDENTITY+'''info registers pc sp xpsr
p tr2_commission_stage
p tr2_commission_sensor_whoami
p tr2_commission_fram_identity_matches
p tr2_commission_fram_rdid
'''+ 'dump binary memory '+win(P/'flash-before.bin')+' 0x08000000 0x08200000\n')
a=(P/'flash-before.bin').read_bytes();assert hashlib.sha256(a).hexdigest()=='c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26'
print('FLASH_IDENTICAL_TO_R4_RESTORED',flush=True)
debug('fram-fresh-check',old,IDENTITY+'''frame function CommissioningReadOnly
p storage
set $r4p_read=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)
p $r4p_read
if $r4p_read != 0
 quit 4
end
'''+ 'dump binary memory '+win(P/'fram-before.bin')+' tr2_commission_fram_backup tr2_commission_fram_backup+262144\n')
b=(P/'fram-before.bin').read_bytes();assert hashlib.sha256(b).hexdigest()=='14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9'
# Now the required full backups exist and agree; only volatile RAM is reused.
debug('sensor-stopped-readonly',old,IDENTITY+'''set $r4p_sensor=(Stm32Iis3dwbVibrationSource*)tr2_commission_fram_backup
set $r4p_id=(int)stm32_iis3dwb_vibration_source_init($r4p_sensor,&hspi3,(GPIO_TypeDef*)0x42020800,512)
p $r4p_id
if $r4p_id != 0
 quit 5
end
set $r4p_buffer=tr2_commission_fram_backup+128
p (int)read_registers($r4p_sensor,15,$r4p_buffer,1)
p (int)read_registers($r4p_sensor,16,$r4p_buffer+1,1)
p (int)read_registers($r4p_sensor,10,$r4p_buffer+2,1)
p (int)read_registers($r4p_sensor,25,$r4p_buffer+3,1)
x/4bx $r4p_buffer
if $r4p_buffer[0] != 0x7B || ($r4p_buffer[1]&0xE0) != 0 || ($r4p_buffer[2]&7) != 0 || ($r4p_buffer[3]&0x20) != 0
 quit 6
end
''')
r=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),flash_bytes=len(a),flash_sha256=hashlib.sha256(a).hexdigest(),fram_bytes=len(b),fram_sha256=hashlib.sha256(b).hexdigest(),fresh_full_reads_match_verified_R4S_backups=True,match_R4S_final=True,UID=['004A003D','38425018','20333157'],STLINK=SN,flash_or_persistent_write=False,reset=False,access='qualified GDB backup/read-only sensor registers; only RAM staging reused',note='Fresh full reads compared to previously verified R4S backups, reused; no flash or writes.')
(P/'preflight-summary.json').write_text(json.dumps(r,indent=2)+'\n');print(r,flush=True)
