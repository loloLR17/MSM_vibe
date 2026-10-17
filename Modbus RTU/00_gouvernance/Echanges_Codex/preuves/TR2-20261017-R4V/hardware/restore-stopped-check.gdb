set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r4u_result
p r4m_cs_closed
p g_serial.tx_busy
p g_serial.tx_fault
p owner.acquisition.fifo.stopped
p owner.acquisition.fifo.stop_error
if owner.acquisition.fifo.running || !owner.acquisition.fifo.stopped || owner.acquisition.fifo.stop_error
 quit 4
end

detach
quit
