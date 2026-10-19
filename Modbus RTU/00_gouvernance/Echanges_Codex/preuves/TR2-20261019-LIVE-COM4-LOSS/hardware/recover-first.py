from hardware import *
e=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/firmware/tr2-live-operator-NON-PRODUCTION.elf')
assert json.loads((P/'preserved-state.json').read_text())['fram_fresh_exact_last_terminal']
debug('recover-stopped-for-causal-test',e,IDENTITY+'''if owner.acquisition.fifo.running || engine.has_active || r4u_result.error != 6
 quit 8
end
monitor reset
''')
time.sleep(12)
