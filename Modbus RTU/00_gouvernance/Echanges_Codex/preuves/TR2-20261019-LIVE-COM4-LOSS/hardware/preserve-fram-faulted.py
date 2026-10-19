from hardware import *
e=Path('/home/lolo/dev/msm/reports/TR2-uxl2-physical/firmware/tr2-live-operator-NON-PRODUCTION.elf')
assert hashlib.sha256((P/'flash-initial-prefix.bin').read_bytes()).hexdigest()=='9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981'
debug('fram-fresh-fault-preserved',e,IDENTITY+'''if owner.acquisition.fifo.running || engine.has_active || r4u_result.error != 6
 quit 7
end
p r4u_result
p work
x/12wx g_serial.uart.Instance
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
if $rr != 0
 quit 4
end
'''+f'dump binary memory {win(P/"fram-initial.bin")} baseline baseline+262144\n')
a=(P/'fram-initial.bin').read_bytes();assert hashlib.sha256(a).hexdigest()=='e18e07adbf12176fa02f1385fa6b4713e5b35e6f101c49959229cf2f1067ec15'
(P/'preserved-state.json').write_text(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),flash_prefix_exact_c36c971=True,fram_fresh_exact_last_terminal=True,fram_sha256=hashlib.sha256(a).hexdigest(),initial_GDB_snapshot_before_any_reset_or_target_function_call=True,owner_stopped=True,engine_has_active=False,journal=94,root=107,epoch=5,run=13,terminal='STOP5/17',reset_or_flash=False),indent=2)+'\n');print('Fresh FRAM unchanged; fault evidence preserved before reset')
