from hardware import *
summary=json.loads((P/'snapshot-resume-summary.json').read_text());assert summary['model_equal']
assert 'stage = 10, error = 0' in (P/'r4p-observation8.log').read_text(errors='replace')
elf=Path('/home/lolo/dev/msm/reports/TR2-r4p/resume-build/tr2-r4p-coexistence-NON-PRODUCTION.elf')
debug('r4p-reboot-reset',elf,IDENTITY+'monitor reset\n')
time.sleep(2)
debug('r4p-reboot-trigger5',elf,IDENTITY+'''p r4_result
p r4m_guard
if r4_result.stage != 1 || r4m_guard.phase != 0
 quit 4
end
set r4_trigger=0x52340005
''')
(P/'r4p-reboot-started.json').write_text(json.dumps(dict(epoch=time.time(),utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
