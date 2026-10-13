from pathlib import Path
import sys,json,datetime,time
P=Path(Path('/tmp/m016-load-proof').read_text());sys.path.insert(0,str(P));from hardware import *
elf=Path('/home/lolo/dev/msm/reports/M016-1bit-load-build/sdmmc_reference.elf');marker=P/'unique-load-attempt.json';assert not marker.exists();marker.write_text(json.dumps({'run':1,'manual_trigger':'4C314C44','one_attempt_only':True,'source_sha':'c6c56b63e02cb78f176f278195dc114a6b7cbe76','utc':datetime.datetime.now(datetime.timezone.utc).isoformat()},indent=2)+'\n')
fields=['load_stage','load_error','load_phase','load_transfer_phase','load_generated','load_produced','load_consumed','load_backlog_max','load_overflow','load_lost','load_producer_done','load_written','load_written_logical','load_verified','load_verified_logical','load_write_chunks','load_read_chunks','load_mismatches','load_first_mismatch','load_first_io_error','load_first_cmd_error','load_max_producer_gap_us','load_generation_us','load_write_elapsed_us','load_verify_elapsed_us','load_clock_hz','load_kernel_hz','load_clkcr','load_bus_card','load_blocks','load_dma_error','load_hal_return','load_last_r1','load_cmd_error']
body=IDENTITY+'''p load_stage
p load_trigger
if load_stage != 2 || load_trigger != 0 || load_generated != 0 || load_written != 0
 quit 4
end
hbreak *0x08001666
set load_trigger=0x4C314C44
continue
'''
for field in fields:body+='printf "METRIC '+field+'=%u\\n", '+field+'\n'
body+='p/x hsd2.ErrorCode\np/x load_cid\nx/2wx 0xE000ED28\ndump binary memory '+win(P/'write-latencies.bin')+' load_write_latency_us load_write_latency_us+4096\n'
start=time.monotonic();print('SINGLE_LOAD_STARTED; no intermediate halt; waiting for terminal',flush=True)
debug('single-load-run',elf,body,timeout=1500)
(P/'run-host-elapsed.json').write_text(json.dumps({'host_wall_seconds_including_debug_setup_and_capture':time.monotonic()-start,'utc_finished':datetime.datetime.now(datetime.timezone.utc).isoformat()},indent=2)+'\n');print((P/'single-load-run.log').read_text(errors='replace'),flush=True)
