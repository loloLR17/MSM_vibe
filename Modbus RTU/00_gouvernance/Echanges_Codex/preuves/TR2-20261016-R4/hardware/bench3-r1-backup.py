from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
debug('bench3-r1-fram-backup',elf,IDENTITY+'''p owner.acquisition.fifo.stopped
if owner.acquisition.fifo.stopped != 1
 quit 7
end
set var fram.timeout_ms=10
set $r4_control=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $r4_control
if $r4_control != 0
 quit 8
end
'''+ 'dump binary memory '+win(P/'fram-after-r1.bin')+' baseline baseline+262144\n')
before=(P/'fram-before.bin').read_bytes();after=(P/'fram-after-r1.bin').read_bytes()
assert before[:122880]==after[:122880] and before[245760:]==after[245760:]
r=dict(sha256=hashlib.sha256(after).hexdigest(),changed_bytes=sum(x!=y for x,y in zip(before,after)),historical_unchanged=True,tail_unchanged=True,sha256_historical=hashlib.sha256(after[:122880]).hexdigest(),sha256_tail=hashlib.sha256(after[245760:]).hexdigest())
(P/'fram-after-r1-summary.json').write_text(json.dumps(r,indent=2)+'\n');print(r,flush=True)
