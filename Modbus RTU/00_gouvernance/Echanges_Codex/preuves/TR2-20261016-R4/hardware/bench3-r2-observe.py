from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
record=json.loads((P/'r2-trigger-host-time.json').read_text());assert (datetime.datetime.now(datetime.timezone.utc)-datetime.datetime.fromisoformat(record['utc'])).total_seconds()>=210
# One observation after the planned unattended sequence, never live polling.
debug('bench3-r2-result',elf,IDENTITY+'''p r4_result
p work
p owner.acquisition.fifo
p configuration
p engine.state
p store.epoch
p bench_log_refused
p bench_preclose_overflow
p hspi1.ErrorCode
''')
