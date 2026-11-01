set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p r3_result
p r3_card
p durable_status
if r3_result.stage!=4 || r3_result.run!=3 || r3_result.error || r3_clock_fault || r3_command || fifo.running || r3_writer.active || q.count || witness_errors || !(*(unsigned int*)0x42021414 &0x1000)
 quit 4
end
if !r3_card.initialized || r3_card.failed || r3_card.sectors!=62333952 || !durable_status.mounted || durable_status.fs_type!=3 || durable_status.error || durable_status.io_uncertain
 quit 5
end
set $cid_rc = (int)'sd_spi.c'::reg_data(&r3_card,10,(unsigned char*)(r3_header+128))
p $cid_rc
if $cid_rc || ((unsigned char*)r3_header)[128]!=0x03 || ((unsigned char*)r3_header)[129]!=0x53 || ((unsigned char*)r3_header)[130]!=0x44 || ((unsigned char*)r3_header)[131]!=0x53 || ((unsigned char*)r3_header)[132]!=0x44 || ((unsigned char*)r3_header)[133]!=0x33 || ((unsigned char*)r3_header)[134]!=0x32 || ((unsigned char*)r3_header)[135]!=0x47 || ((unsigned char*)r3_header)[136]!=0x85 || ((unsigned char*)r3_header)[137]!=0x8b || ((unsigned char*)r3_header)[138]!=0xf0 || ((unsigned char*)r3_header)[139]!=0x92 || ((unsigned char*)r3_header)[140]!=0x3c || ((unsigned char*)r3_header)[141]!=0x01 || ((unsigned char*)r3_header)[142]!=0x92 || ((unsigned char*)r3_header)[143]!=0x7b
 quit 6
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/hardware/prepare-final-image-readback.bin 0x08000000 0x800cf28
set {char[15]} r3_header = "0:/N8F0005.TR2"
set $rc = (int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $rc
if $rc != 4
 if $rc == 0
  call (int)f_close(&'fat_volume.c'::file)
 end
 quit 7
end
set {char[3]} r3_header = "0:"
set $fr = (int)f_getfree((char*)r3_header,(DWORD*)(r3_header+64),(FATFS**)(r3_header+72))
p $fr
p *(unsigned int*)(r3_header+64)
if $fr || *(unsigned int*)(r3_header+64)<20480
 quit 8
end

detach
quit
