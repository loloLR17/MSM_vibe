from hardware import *
elf=Path('/home/lolo/dev/msm/reports/TR2-r4p/resume-build/tr2-r4p-coexistence-NON-PRODUCTION.elf')
log=(P/'r4p-observation8.log').read_text(errors='replace')
assert 'stage = 10, error = 0' in log or 'running = false' in log,'must observe terminal stopped before snapshot'
debug('r4p-fram-after-resume',elf,IDENTITY+'''p r4_result
p owner.acquisition.fifo.running
if owner.acquisition.fifo.running
 quit 4
end
set fram.timeout_ms=10
set $r=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r
if $r != 0
 quit 5
end
'''+f'dump binary memory {win(P/"fram-after-resume.bin")} baseline baseline+262144\n',timeout=60)
a=(P/'fram-before.bin').read_bytes();b=(P/'fram-after-resume.bin').read_bytes()
assert len(b)==262144 and a[:122880]==b[:122880] and a[245760:]==b[245760:]
result=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),sha256=hashlib.sha256(b).hexdigest(),protected_equal=True,model_equal=b==Path('/home/lolo/dev/msm/reports/TR2-r4p/expected-final-MODEL-ONLY.bin').read_bytes())
(P/'snapshot-resume-summary.json').write_text(json.dumps(result,indent=2)+'\n');print(result)
