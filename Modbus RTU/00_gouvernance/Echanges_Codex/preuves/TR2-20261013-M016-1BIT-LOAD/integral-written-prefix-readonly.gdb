set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
p load_stage
p load_error
p load_written
if load_stage != 99 || load_error != 3 || load_producer_enabled != 0 || load_written != 1310720
 quit 4
end
set $read_ok=transfer(0,rx,2048,65536)
printf "READ_CHUNK 0 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-000.bin rx rx+65536
set $read_ok=transfer(0,rx,2176,65536)
printf "READ_CHUNK 1 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-001.bin rx rx+65536
set $read_ok=transfer(0,rx,2304,65536)
printf "READ_CHUNK 2 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-002.bin rx rx+65536
set $read_ok=transfer(0,rx,2432,65536)
printf "READ_CHUNK 3 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-003.bin rx rx+65536
set $read_ok=transfer(0,rx,2560,65536)
printf "READ_CHUNK 4 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-004.bin rx rx+65536
set $read_ok=transfer(0,rx,2688,65536)
printf "READ_CHUNK 5 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-005.bin rx rx+65536
set $read_ok=transfer(0,rx,2816,65536)
printf "READ_CHUNK 6 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-006.bin rx rx+65536
set $read_ok=transfer(0,rx,2944,65536)
printf "READ_CHUNK 7 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-007.bin rx rx+65536
set $read_ok=transfer(0,rx,3072,65536)
printf "READ_CHUNK 8 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-008.bin rx rx+65536
set $read_ok=transfer(0,rx,3200,65536)
printf "READ_CHUNK 9 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-009.bin rx rx+65536
set $read_ok=transfer(0,rx,3328,65536)
printf "READ_CHUNK 10 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-010.bin rx rx+65536
set $read_ok=transfer(0,rx,3456,65536)
printf "READ_CHUNK 11 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-011.bin rx rx+65536
set $read_ok=transfer(0,rx,3584,65536)
printf "READ_CHUNK 12 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-012.bin rx rx+65536
set $read_ok=transfer(0,rx,3712,65536)
printf "READ_CHUNK 13 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-013.bin rx rx+65536
set $read_ok=transfer(0,rx,3840,65536)
printf "READ_CHUNK 14 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-014.bin rx rx+65536
set $read_ok=transfer(0,rx,3968,65536)
printf "READ_CHUNK 15 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-015.bin rx rx+65536
set $read_ok=transfer(0,rx,4096,65536)
printf "READ_CHUNK 16 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-016.bin rx rx+65536
set $read_ok=transfer(0,rx,4224,65536)
printf "READ_CHUNK 17 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-017.bin rx rx+65536
set $read_ok=transfer(0,rx,4352,65536)
printf "READ_CHUNK 18 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-018.bin rx rx+65536
set $read_ok=transfer(0,rx,4480,65536)
printf "READ_CHUNK 19 result=%u\n", $read_ok
if $read_ok != 1
 quit 7
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-1BIT-LOAD-20261013T215507Z\prefix-read-019.bin rx rx+65536
p load_stage
p load_error
p/x hsd2.ErrorCode
p/x load_last_r1
x/2wx 0xE000ED28

detach
quit
