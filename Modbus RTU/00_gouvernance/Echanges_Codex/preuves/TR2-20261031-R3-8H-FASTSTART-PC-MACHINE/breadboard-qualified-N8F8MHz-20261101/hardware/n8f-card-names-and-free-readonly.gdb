set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p durable_status
if r3_result.stage!=4 || r3_result.error || r3_result.run!=1 || r3_command || fifo.running || r3_writer.active || r3_clock_fault || durable_status.fs_type!=3 || durable_status.cluster_sectors!=64
 quit 5
end
set {char[15]} r3_header = "0:/N8F0003.TR2"
set $name3 = (int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $name3
if $name3 != 4
 if $name3 == 0
  call (int)f_close(&'fat_volume.c'::file)
 end
 quit 6
end
set {char[15]} r3_header = "0:/N8F0005.TR2"
set $name5 = (int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $name5
if $name5 != 4
 if $name5 == 0
  call (int)f_close(&'fat_volume.c'::file)
 end
 quit 7
end
set {char[3]} r3_header = "0:"
set $free_rc = (int)f_getfree((char*)r3_header,(DWORD*)(r3_header+64),(FATFS**)(r3_header+72))
p $free_rc
p *(unsigned int*)(r3_header+64)
if $free_rc || *(unsigned int*)(r3_header+64)<20484
 quit 8
end
p r3_card
p r3_clock_fault

detach
quit
