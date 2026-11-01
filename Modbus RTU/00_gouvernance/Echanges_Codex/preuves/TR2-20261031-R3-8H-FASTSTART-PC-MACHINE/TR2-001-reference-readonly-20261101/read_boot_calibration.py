from pathlib import Path
import importlib.util,json
P=Path(__file__).parent
source=Path('/home/lolo/dev/msm/reports/TR2-r3-N8E-duplex4-20261101/hardware_second.py').read_text().split('def flash(')[0]
source=source.replace("P=Path('/home/lolo/dev/msm/reports/TR2-r3-N8E-duplex4-20261101/hardware')",f"P=Path({str(P/'hardware')!r})").replace('0032001F3235510C37333439','003500463235510B37333439').replace('0x001B0046','0x004A003D')
f=P/'hardware_primary_readonly.py';assert not f.exists();f.write_text(source)
spec=importlib.util.spec_from_file_location('hp',f);h=importlib.util.module_from_spec(spec);spec.loader.exec_module(h)
elf=Path('/home/lolo/dev/msm/reports/TR2-accel-fs8g-ac/firmware/tr2-live-operator-NON-PRODUCTION.elf')
body=h.IDENTITY+'''p r4u_result
p r4r_result
p diag_fifo.running
p diag_fifo.error
p diag_fifo.stop_error
p diag_fifo.total_xyz
p diag_fifo.full_scale
p diag_fifo.parser
p owner.acquisition.fifo.running
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
p bench_trace_count
p bench_payload_count
p bench_preclose_count
p work.error
p configuration.active.payload.full_scale_code
if r4u_result.stage!=7 || r4u_result.error || r4r_result.error || diag_fifo.running || diag_fifo.error || diag_fifo.stop_error || owner.acquisition.fifo.running || owner.acquisition.fifo.error || owner.acquisition.fifo.stop_error || r4r_result.calibration_residual_us>100 || r4r_result.brackets<100 || r4r_result.anchors<100 || r4r_result.counter_mismatch || r4r_result.reserved_nonzero
 quit 8
end
'''
for name,start,end in [('calibration-result','&r4r_result','((char*)&r4r_result)+sizeof(r4r_result)'),('calibration-brackets','r4r_brackets','r4r_brackets+r4r_result.brackets'),('calibration-first-raw','r4r_raw','r4r_raw+r4r_result.raw_words*7')]:
 body+=f'dump binary memory {h.win(P/(name+".bin")).replace(chr(92),"/")} {start} {end}\n'
body+='''if bench_trace_count<=1024 && bench_payload_count<=65536
'''
body+=f'dump binary memory {h.win(P/"boot-transport-trace.bin").replace(chr(92),"/")} bench_trace bench_trace+bench_trace_count\n'
body+=f'dump binary memory {h.win(P/"boot-transport-payload.bin").replace(chr(92),"/")} bench_payload bench_payload+bench_payload_count\nend\n'
h.debug('TR2-001-boot-calibration-RAM-only',elf,body)
print((P/'hardware/TR2-001-boot-calibration-RAM-only.log').read_text())
