set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if r3_result.error || r3_result.stage!=4 || r3_result.run!=1 || fifo.running || r3_writer.active
 quit 12
end
set {char[15]} r3_header = "0:/OBE0003.TR2"
set $rc=(int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $rc
if $rc!=4
 if $rc==0
  call (int)f_close(&'fat_volume.c'::file)
 end
 quit 14
end
set {char[15]} r3_header = "0:/OBE0005.TR2"
set $rc=(int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $rc
if $rc!=4
 if $rc==0
  call (int)f_close(&'fat_volume.c'::file)
 end
 quit 14
end
set {char[3]} r3_header = "0:"
set $fr=(int)f_getfree((char*)r3_header,(DWORD*)(r3_header+64),(FATFS**)(r3_header+72))
p $fr
p *(unsigned int*)(r3_header+64)
if $fr || *(unsigned int*)(r3_header+64)<20484
 quit 15
end

detach
quit
