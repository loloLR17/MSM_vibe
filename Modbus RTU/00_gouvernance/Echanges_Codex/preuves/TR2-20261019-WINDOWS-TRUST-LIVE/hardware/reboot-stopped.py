from hardware import *
e=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/firmware/tr2-live-operator-NON-PRODUCTION.elf')
assert json.loads((P/'cycle1-audit-summary.json').read_text())['historical_prefix_122880_unchanged']
debug('stopped-software-reset',e,IDENTITY+'''if owner.acquisition.fifo.running
 quit 7
end
monitor reset
''')
time.sleep(15)
debug('autonomous-reboot-stopped',e,IDENTITY+'''p r4u_result
p engine.last
p engine.state
p owner.acquisition.fifo.running
p store.generation
p live.generation
if r4u_result.stage != 7 || r4u_result.error != 0 || owner.acquisition.fifo.running || store.generation != 86 || live.generation != 99
 quit 8
end
''')
