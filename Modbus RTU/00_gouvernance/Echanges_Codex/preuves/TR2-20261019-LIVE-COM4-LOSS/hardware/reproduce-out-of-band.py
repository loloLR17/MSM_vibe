from hardware import *
e=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/firmware/tr2-live-operator-NON-PRODUCTION.elf')
debug('reproduce-callback-read-once',e,IDENTITY+'''if owner.acquisition.fifo.running || engine.has_active || r4u_result.error != 0
 quit 7
end
tbreak u_service
continue
p work
if !work.servicing || owner.acquisition.fifo.running || engine.has_active
 quit 8
end
bt
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
if $rr != 0
 quit 9
end
'''+f'dump binary memory {win(P/"reproduction-fram.bin")} baseline baseline+262144\n')
assert hashlib.sha256((P/'reproduction-fram.bin').read_bytes()).hexdigest()=='e18e07adbf12176fa02f1385fa6b4713e5b35e6f101c49959229cf2f1067ec15'
time.sleep(1)
debug('reproduced-fault-no-call',e,IDENTITY+'''bt
p r4u_result
p work
p engine.state
p engine.last
p engine.has_active
p owner.acquisition.fifo.running
if r4u_result.error != 6 || !work.deadline_missed || work.max_service_us < 800000 || owner.acquisition.fifo.running || engine.has_active
 quit 10
end
''')
print('CALLBACK READ REPRODUCES THE SAME LATCHED FAULT, NO PERSISTENT CHANGE')
