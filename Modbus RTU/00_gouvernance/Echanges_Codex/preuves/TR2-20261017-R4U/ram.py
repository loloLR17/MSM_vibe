from hardware import *
R=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4U-live-runtime');B=P.parent/'probe-build-01';elf=B/'tr2-r4u-live-runtime-NON-PRODUCTION.elf';image=elf.with_suffix('.bin');code=subprocess.check_output(['git','rev-parse','HEAD'],cwd=R,text=True).strip()
assert code=='43840fedf6c7e2e40d5a49eeea822d5d91d1ee55'
# Re-read full checkpoint files from fetched GitHub reference, not working copies.
count=0
for name in subprocess.check_output(['git','diff-tree','--no-commit-id','--name-only','-r','-z',code],cwd=R).decode().split('\0'):
 if not name:continue
 data=subprocess.check_output(['git','show','origin/main:'+name],cwd=R);assert data==(R/name).read_bytes();count+=1
(P.parent/'checkpoint-readback.json').write_text(json.dumps(dict(sha=code,files=count,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
assert hashlib.sha256(image.read_bytes()).hexdigest()=='08f28d85efee19ea966481e7b15dd9f89f331023b5978a44aa49891e09ac3ef8'
# Before normal programming, fresh identity and Flash check without reset.
m016=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
debug('before-programming-identity',m016,IDENTITY+f'dump binary memory {win(P/"flash-before-programming.bin")} 0x08000000 0x08200000\n',35)
assert (P/'flash-before-programming.bin').read_bytes()==(P/'flash-before.bin').read_bytes()
flash('image01-flash',image,code)
debug('image01-boot-check',elf,IDENTITY+f'''dump binary memory {win(P/'image01-readback.bin')} 0x08000000 0x08000000+{image.stat().st_size}
monitor reset
shell sleep 1
p r4u_result
if r4u_result.stage != 1 || r4u_result.error != 0
 quit 4
end
''',40)
assert (P/'image01-readback.bin').read_bytes()==image.read_bytes()
body=IDENTITY+'''if r4u_result.stage != 1 || r4u_result.error
 quit 4
end
break r4u_checkpoint
set r4_trigger=0x52345501
continue
p r4u_result
p r4_result
p work
p runtime
p metric
p event_time
p r4r_result
p r4s_result
p engine
p live.generation
p store.generation
p store.epoch
p store.effect
p owner.acquisition.fifo
'''+f'''set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
dump binary memory {win(P/'ram-fram.bin')} baseline baseline+262144
'''+'''if $rr || r4u_result.stage != 3 || r4u_result.error || work.error || r4_result.max_gap_us > 5000 || r4_result.write_bytes
 quit 5
end
'''
debug('ram-phase',elf,body,60)
assert (P/'ram-fram.bin').read_bytes()==(P/'fram-before.bin').read_bytes()
print('RAM FRAM exact',flush=True)
