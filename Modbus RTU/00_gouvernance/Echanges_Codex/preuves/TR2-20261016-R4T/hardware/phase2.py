from hardware import *
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4T-monotonic-recovery');local=P.parent
pre=json.loads((P/'preflight-summary.json').read_text());assert pre['match_R4S_final']
assert subprocess.check_output(['git','status','--porcelain'],cwd=root,text=True)==''
sha=subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip()
assert sha==json.loads((local/'latest-readback.json').read_text())['sha']
inv=(local/'stlink-inventory.log').read_text();assert inv.count('ST-LINK SN')==1 and SN in inv
image=local/'probe-build-01/tr2-r4t-monotone-recovery-NON-PRODUCTION.bin';elf=image.with_suffix('.elf')
r=dict(code_sha=sha,bin_bytes=image.stat().st_size,bin_sha256=hashlib.sha256(image.read_bytes()).hexdigest(),elf_sha256=hashlib.sha256(elf.read_bytes()).hexdigest(),profile=(image.parent/'build-profile.txt').read_text(),compiler=subprocess.check_output(['arm-none-eabi-gcc','--version'],text=True),same_sources_after_commit_noop_build=(local/'probe-code-identity.log').read_text(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(local/'physical-build.json').write_text(json.dumps(r,indent=2)+'\n')
flash('phase2-flash',image,sha)
debug('phase2-reset-readback',elf,IDENTITY+f'dump binary memory {win(P/"probe-readback.bin")} 0x08000000 {hex(0x08000000+image.stat().st_size)}\nmonitor reset\n')
assert (P/'probe-readback.bin').read_bytes()==image.read_bytes();time.sleep(2)
debug('phase2-retire-result',elf,IDENTITY+'''p r4t_recovery
if r4t_recovery.stage != 1 || r4t_recovery.error
 quit 4
end
break r4r_checkpoint
set r4s_backup_token=0x53345342
set r4_trigger=0x5234000D
continue
p r4t_recovery
p r4_result
p work
p engine
p store
p retirement
p r4m_guard
p owner.acquisition.fifo
set $r=(int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 6
end
'''+f'dump binary memory {win(P/"fram-after-maintenance.bin")} baseline baseline+262144\n',timeout=240)
a=(P/'fram-after-maintenance.bin').read_bytes();expected=(local/'model-renew.bin').read_bytes()
r=dict(physical_full_sha256=hashlib.sha256(a).hexdigest(),model_full_sha256=hashlib.sha256(expected).hexdigest(),full_exact_model=a==expected,protected_exact=a[:122880]==(P/'fram-before.bin').read_bytes()[:122880] and a[245760:]==(P/'fram-before.bin').read_bytes()[245760:],utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(P/'phase2-comparison.json').write_text(json.dumps(r,indent=2)+'\n');assert r['full_exact_model'] and r['protected_exact'],r
print('PHASE2 FULL FRAM EXACT MODEL',r,flush=True)
