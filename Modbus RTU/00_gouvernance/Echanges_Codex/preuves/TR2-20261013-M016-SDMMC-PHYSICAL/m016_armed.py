from pathlib import Path
import sys,time,hashlib,json
P=Path(Path('/tmp/m016-sd-physical-proof').read_text());sys.path.insert(0,str(P));from hardware import *
b=Path('/home/lolo/dev/msm/reports/M016-sdmmc-reference-armed-build');im=b/'sdmmc_reference.bin';assert hashlib.sha256(im.read_bytes()).hexdigest()=='c183e3296715af7aebee83c316532a237b7ab0535943087ef92177314f8011b9'
(P/'media-authorization.json').write_text(json.dumps({'second_uid':'001B00463842501820333157','cid':['03534453','44333247','858BF092','3C01927A'],'card_blocks':62333952,'block_bytes':512,'test_lba':2048,'test_blocks':8,'bytes_written_each_round':4096,'rounds':3,'authorization':'user and current contract: entire second-bench test SD disposable','no_erase_or_format_needed':True},indent=2)+'\n')
flash('armed-flash',im,'077835157fbdefecb4ebc2a416cbf82d1f8891d7')
debug('armed-readback-reset',b/'sdmmc_reference.elf',IDENTITY+'dump binary memory '+win(P/'armed-flash-readback.bin')+f' 0x08000000 0x{0x08000000+im.stat().st_size:08X}\nmonitor reset\n');assert (P/'armed-flash-readback.bin').read_bytes()==im.read_bytes();time.sleep(3)
body=IDENTITY+'''p bench_stage
p/x bench_cid
p/x bench_bus_width
p bench_blocks
if bench_stage != 2 || bench_blocks != 62333952 || bench_bus_width != 0x4000 || bench_trigger != 0
 quit 4
end
if bench_cid[0] != 0x03534453 || bench_cid[1] != 0x44333247 || bench_cid[2] != 0x858BF092 || bench_cid[3] != 0x3C01927A
 quit 5
end
set bench_expected_cid[0]=0x03534453
set bench_expected_cid[1]=0x44333247
set bench_expected_cid[2]=0x858BF092
set bench_expected_cid[3]=0x3C01927A
set bench_trigger=0x4D313642
'''
marker=P/'write-attempt01.json';assert not marker.exists();marker.write_text(json.dumps({'trigger':'4D313642','unique':True,'utc':datetime.datetime.now(datetime.timezone.utc).isoformat()},indent=2)+'\n');debug('write-attempt01-trigger',b/'sdmmc_reference.elf',body);time.sleep(5)
debug('write-attempt01-result',b/'sdmmc_reference.elf',IDENTITY+'''p bench_stage
p bench_phase
p bench_rounds
p/x bench_error
p/x bench_refusal
p/x bench_dma_error
p bench_mismatch
p bench_elapsed_ms
p bench_write_ms
p bench_read_ms
p/x bench_last_r1
p/x bench_cmd13_error
p/x bench_hal_return
p/x hsd2.ErrorCode
p/x bench_clkcr
p bench_kernel_hz
p bench_clock_hz
x/2wx 0xE000ED28
'''+ 'dump binary memory '+win(P/'attempt01-rx.bin')+' rx rx+4096\n')
print((P/'write-attempt01-result.log').read_text(errors='replace'))
