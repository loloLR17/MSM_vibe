from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
debug('bench4-fram-preserve-after-wait-observed',elf,IDENTITY+'''p r4_result
p owner.acquisition.fifo.running
set $r4_stop=(int)fifo_live_stop(&owner.acquisition.fifo)
p $r4_stop
if $r4_stop != 0
 quit 7
end
set var fram.timeout_ms=10
set $r4_control=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r4_control
if $r4_control != 0
 quit 8
end
'''+ 'dump binary memory '+win(P/'fram-after-r2-indeterminate.bin')+' baseline baseline+262144\n')
a=(P/'fram-before.bin').read_bytes();b=(P/'fram-after-r2-indeterminate.bin').read_bytes();assert a[:122880]==b[:122880] and a[245760:]==b[245760:]
r=dict(sha256=hashlib.sha256(b).hexdigest(),bytes=len(b),historical_unchanged=True,tail_unchanged=True,changed_bytes=sum(x!=y for x,y in zip(a,b)),write_scope='LIVE only',state='STARTED indeterminate from bench4-r2-result.log; no erase/replay',note='WAIT freshly observed after previous terminal reading, cause of reinitialization not established; no assumption of power cycle')
(P/'fram-after-r2-summary.json').write_text(json.dumps(r,indent=2)+'\n');print(r,flush=True)
