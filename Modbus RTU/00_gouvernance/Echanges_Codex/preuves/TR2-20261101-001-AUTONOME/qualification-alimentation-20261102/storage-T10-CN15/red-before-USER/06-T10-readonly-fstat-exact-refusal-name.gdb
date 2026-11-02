set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:23336
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p sizeof(FILINFO)
if sizeof(FILINFO)>400 || fifo.running || r3_writer.active || r3_card.failed || r3_card.writes
 quit 12
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\scratch-before.bin r3_header r3_header+512
restore \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\filename-RAM.bin binary r3_header+400
set $fr=(int)f_stat((char *)(r3_header+400),(FILINFO *)r3_header)
printf "FILE_STAT_RC=%d\n",$fr
if $fr==0
 printf "FILE_SIZE=%llu FILE_ATTR=%u\n",(unsigned long long)((FILINFO *)r3_header)->fsize,((FILINFO *)r3_header)->fattrib
 p ((FILINFO *)r3_header)->fname
end
restore \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\scratch-before.bin binary r3_header
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-001-autonome-reducteur-20261101\qualification-alimentation-20261102\storage-T10-CN15\red-before-USER\scratch-after.bin r3_header r3_header+512
p r3_card.writes
p r3_writer.active
p r3_writer.uncertain
p/x *(unsigned int *)0x42021414

detach
quit
