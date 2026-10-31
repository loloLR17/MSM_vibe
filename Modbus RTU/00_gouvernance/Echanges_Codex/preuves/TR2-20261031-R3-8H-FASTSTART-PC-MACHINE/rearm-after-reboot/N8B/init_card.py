from pathlib import Path
import json,struct
import hardware_second as h
import run_capture as c
P=Path(__file__).parent
r=c.run('n8b-card-calibrate',0,1)
b=(P/'n8b-card-calibrate/card.bin').read_bytes();assert b[16:32].hex()=='0353445344333247858bf0923c01927b' and b[32:48].hex()=='400e00325b590000edc87f800a4040c3' and struct.unpack_from('<II',b,48)==(0xc0ff8000,62333952)
s=(P/'n8b-card-calibrate/durable-status.bin').read_bytes();assert struct.unpack_from('<III',s)==(1,0,0) and r['who']==0x7b and r['sd_sectors']==62333952
# f_open FA_READ only; FR_NO_FILE=4, no filesystem writes or name replacement.
body=h.IDENTITY+'''p durable_status
if r3_result.stage!=4 || r3_result.error || r3_result.run!=1 || r3_command || fifo.running || r3_writer.active || r3_clock_fault || durable_status.fs_type!=3 || durable_status.cluster_sectors!=64
 quit 5
end
set {char[15]} r3_header = "0:/N8B0003.TR2"
set $name3 = (int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $name3
if $name3 != 4
 if $name3 == 0
  call (int)f_close(&'fat_volume.c'::file)
 end
 quit 6
end
set {char[15]} r3_header = "0:/N8B0005.TR2"
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
'''
h.debug('n8b-card-names-and-free-readonly',P/'arm/r3_coex.elf',body,timeout=55)
(P/'CARD_PREFLIGHT.json').write_text(json.dumps(dict(CID=b[16:32].hex(),CSD=b[32:48].hex(),sectors=62333952,FAT32=True,cluster_bytes=32768,both_N8B_names_absent=True,free_clusters_at_least=20484,names_checked_via_FatFs_read_only=True,no_format=True,WHO=123),indent=2)+'\n')
print('WHO/CID/CSD/capacity/FAT32 exact; both new names absent; sufficient free space; no format/write',flush=True)
