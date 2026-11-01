set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
if r3_result.stage!=4 || r3_result.run!=3 || r3_result.error || fifo.running || r3_writer.active || r3_command || r3_clock_fault || q.count || r3_card.failed
 quit 8
end
set $writes_before=r3_card.writes
set {char[15]} r3_header="0:/N8F0003.TR2"
set $fr=(int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $fr
if $fr || 'fat_volume.c'::file.obj.objsize!=131072
 quit 9
end
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/00.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/01.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/02.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/03.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/04.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/05.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/06.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/07.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/08.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/09.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/10.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/11.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/12.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/13.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/14.bin r3_raw (r3_raw+8192)
set $rr=(int)f_read(&'fat_volume.c'::file,r3_raw,8192,(unsigned int*)(r3_header+64))
if $rr || *(unsigned int*)(r3_header+64)!=8192
 quit 10
end
dump binary memory //wsl.localhost/Debian/home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/file-chunks/15.bin r3_raw (r3_raw+8192)
set $closed=(int)f_close(&'fat_volume.c'::file)
p $closed
p r3_card.writes
p r3_card.reads
p fifo.running
p r3_writer.active
p/x *(unsigned int*)0x42021414
if $closed || r3_card.writes!=$writes_before || r3_card.failed || fifo.running || r3_writer.active || !(*(unsigned int*)0x42021414 &0x1000)
 quit 11
end

detach
quit
