set pagination off
set confirm off
set remotetimeout 10
target extended-remote localhost:61234
x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x004A003D || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
set variable tr2_commission_sd_read_lba=16512
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 2
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-001.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=16640
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 3
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-002.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=16768
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 4
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-003.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=16896
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 5
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-004.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=17024
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 6
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-005.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=17152
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 7
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-006.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=17280
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 8
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-007.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=17408
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 9
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-008.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=17536
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 10
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-009.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=17664
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 11
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-010.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=17792
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 12
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-011.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=17920
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 13
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-012.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=18048
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 14
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-013.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=18176
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 15
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-014.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=18304
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 16
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-015.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=18432
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 17
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-016.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=18560
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 18
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-017.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=18688
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 19
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-018.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=18816
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 20
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-019.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=18944
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 21
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-020.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=19072
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 22
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-021.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=19200
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 23
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-022.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=19328
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 24
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-023.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=19456
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 25
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-024.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=19584
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 26
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-025.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=19712
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 27
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-026.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=19840
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 28
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-027.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=19968
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 29
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-028.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=20096
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 30
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-029.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=20224
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 31
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-030.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=20352
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 32
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-031.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=20480
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 33
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-032.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=20608
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 34
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-033.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=20736
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 35
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-034.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=20864
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 36
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-035.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=20992
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 37
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-036.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=21120
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 38
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-037.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=21248
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 39
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-038.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=21376
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 40
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-039.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=21504
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 41
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-040.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=21632
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 42
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-041.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=21760
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 43
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-042.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=21888
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 44
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-043.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=22016
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 45
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-044.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=22144
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 46
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-045.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=22272
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 47
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-046.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=22400
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 48
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-047.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=22528
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 49
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-048.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=22656
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 50
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-049.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=22784
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 51
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-050.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=22912
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 52
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-051.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=23040
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 53
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-052.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=23168
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 54
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-053.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=23296
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 55
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-054.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=23424
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 56
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-055.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=23552
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 57
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-056.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=23680
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 58
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-057.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=23808
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 59
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-058.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=23936
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 60
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-059.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=24064
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 61
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-060.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=24192
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 62
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-061.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=24320
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 63
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-062.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=24448
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 64
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-063.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=24576
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 65
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-064.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=24704
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 66
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-065.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=24832
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 67
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-066.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=24960
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 68
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-067.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=25088
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 69
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-068.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=25216
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 70
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-069.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=25344
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 71
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-070.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=25472
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 72
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-071.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=25600
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 73
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-072.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=25728
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 74
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-073.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=25856
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 75
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-074.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=25984
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 76
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-075.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=26112
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 77
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-076.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=26240
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 78
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-077.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=26368
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 79
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-078.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=26496
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 80
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-079.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=26624
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 81
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-080.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=26752
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 82
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-081.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=26880
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 83
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-082.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=27008
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 84
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-083.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=27136
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 85
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-084.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=27264
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 86
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-085.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=27392
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 87
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-086.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=27520
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 88
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-087.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=27648
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 89
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-088.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=27776
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 90
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-089.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=27904
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 91
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-090.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=28032
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 92
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-091.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=28160
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 93
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-092.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=28288
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 94
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-093.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=28416
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 95
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-094.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=28544
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 96
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-095.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=28672
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 97
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-096.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=28800
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 98
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-097.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=28928
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 99
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-098.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=29056
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 100
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-099.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=29184
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 101
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-100.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=29312
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 102
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-101.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=29440
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 103
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-102.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=29568
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 104
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-103.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=29696
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 105
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-104.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=29824
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 106
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-105.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=29952
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 107
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-106.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=30080
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 108
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-107.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=30208
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 109
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-108.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=30336
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 110
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-109.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=30464
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 111
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-110.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=30592
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 112
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-111.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=30720
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 113
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-112.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=30848
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 114
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-113.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=30976
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 115
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-114.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=31104
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 116
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-115.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=31232
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 117
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-116.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=31360
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 118
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-117.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=31488
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 119
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-118.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=31616
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 120
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-119.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=31744
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 121
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-120.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=31872
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 122
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-121.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=32000
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 123
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-122.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=32128
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 124
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-123.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=32256
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 125
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-124.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=32384
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 126
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-125.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=32512
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 127
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-126.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=32640
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 128
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-127.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=32768
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 129
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-128.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=32896
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 130
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-129.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=33024
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 131
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-130.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=33152
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 132
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-131.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=33280
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 133
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-132.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=33408
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 134
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-133.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=33536
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 135
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-134.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=33664
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 136
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-135.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=33792
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 137
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-136.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=33920
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 138
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-137.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=34048
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 139
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-138.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=34176
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 140
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-139.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=34304
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 141
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-140.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=34432
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 142
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-141.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=34560
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 143
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-142.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=34688
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 144
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-143.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=34816
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 145
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-144.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=34944
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 146
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-145.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=35072
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 147
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-146.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=35200
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 148
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-147.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=35328
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 149
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-148.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=35456
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 150
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-149.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=35584
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 151
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-150.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=35712
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 152
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-151.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=35840
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 153
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-152.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=35968
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 154
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-153.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=36096
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 155
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-154.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=36224
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 156
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-155.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=36352
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 157
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-156.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=36480
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 158
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-157.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=36608
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 159
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-158.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=36736
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 160
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-159.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=36864
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 161
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-160.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=36992
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 162
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-161.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=37120
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 163
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-162.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=37248
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 164
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-163.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=37376
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 165
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-164.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=37504
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 166
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-165.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=37632
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 167
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-166.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=37760
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 168
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-167.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=37888
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 169
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-168.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=38016
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 170
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-169.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=38144
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 171
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-170.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=38272
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 172
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-171.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=38400
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 173
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-172.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=38528
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 174
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-173.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=38656
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 175
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-174.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=38784
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 176
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-175.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=38912
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 177
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-176.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=39040
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 178
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-177.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=39168
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 179
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-178.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=39296
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 180
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-179.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=39424
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 181
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-180.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=39552
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 182
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-181.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=39680
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 183
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-182.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=39808
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 184
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-183.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=39936
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 185
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-184.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=40064
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 186
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-185.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=40192
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 187
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-186.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=40320
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 188
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-187.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=40448
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 189
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-188.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=40576
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 190
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-189.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=40704
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 191
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-190.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=40832
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 192
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-191.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=40960
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 193
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-192.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=41088
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 194
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-193.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=41216
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 195
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-194.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=41344
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 196
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-195.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=41472
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 197
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-196.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=41600
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 198
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-197.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=41728
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 199
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-198.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=41856
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 200
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-199.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=41984
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 201
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-200.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=42112
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 202
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-201.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=42240
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 203
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-202.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=42368
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 204
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-203.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=42496
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 205
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-204.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=42624
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 206
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-205.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=42752
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 207
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-206.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=42880
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 208
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-207.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=43008
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 209
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-208.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=43136
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 210
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-209.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=43264
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 211
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-210.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=43392
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 212
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-211.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=43520
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 213
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-212.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=43648
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 214
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-213.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=43776
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 215
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-214.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=43904
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 216
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-215.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=44032
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 217
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-216.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=44160
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 218
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-217.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=44288
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 219
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-218.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=44416
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 220
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-219.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=44544
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 221
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-220.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=44672
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 222
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-221.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=44800
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 223
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-222.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=44928
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 224
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-223.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=45056
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 225
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-224.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=45184
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 226
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-225.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=45312
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 227
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-226.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=45440
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 228
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-227.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=45568
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 229
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-228.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=45696
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 230
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-229.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=45824
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 231
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-230.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=45952
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 232
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-231.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=46080
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 233
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-232.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=46208
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 234
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-233.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=46336
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 235
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-234.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=46464
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 236
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-235.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=46592
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 237
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-236.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=46720
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 238
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-237.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=46848
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 239
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-238.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=46976
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 240
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-239.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=47104
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 241
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-240.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=47232
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 242
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-241.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=47360
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 243
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-242.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=47488
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 244
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-243.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=47616
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 245
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-244.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=47744
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 246
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-245.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=47872
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 247
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-246.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=48000
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 248
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-247.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=48128
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 249
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-248.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=48256
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 250
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-249.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=48384
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 251
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-250.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=48512
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 252
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-251.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=48640
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 253
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-252.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=48768
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 254
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-253.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=48896
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 255
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-254.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536
set variable tr2_commission_sd_read_lba=49024
set variable tr2_commission_sd_read_blocks=128
set variable tr2_commission_sd_read_trigger=0x4D313653
thbreak CommissioningReadChunk
continue
finish
if tr2_commission_sd_read_result != 0 || tr2_commission_sd_read_done != 256
 quit 4
end
dump binary memory \\wsl.localhost\Debian\home\lolo\dev\msm\reports\TR2-M016-storage-20261013T172331Z\candidate-255.bin tr2_commission_sd_chunk tr2_commission_sd_chunk+65536

detach
quit
