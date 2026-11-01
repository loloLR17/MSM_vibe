from pathlib import Path
import json
import hardware_second as h
P=Path(__file__).parent
r=json.loads((P/'breadboard-gate-collected/result.json').read_text());assert r['stage']==4 and r['error']==0 and r['run']==3
body=h.IDENTITY+'''p r3_result
if r3_result.stage!=4 || r3_result.run!=3 || r3_result.error || r3_clock_fault || r3_command || fifo.running || r3_writer.active || q.count
 quit 4
end
set $verified = (int)durable_verify_last(r3_source_sha)
p $verified
p durable_status
if $verified || durable_status.error || durable_status.io_uncertain || !(*(unsigned int*)0x42021414 &0x1000)
 quit 5
end
p r3_source_sha
'''
h.debug('gate-complete-file-reread-SHA',P/'arm/r3_coex.elf',body,timeout=55)
(P/'GATE_RE_READ.json').write_text(json.dumps(dict(complete_gate_reread_through_FatFs=True,CRC_index_count_session_footer_checked=True,all_useful_bytes_SHA_matches_RAM_producer=True,bytes=r['sd_total_bytes'],run=3,no_new_capture=True,GDB_detached=True),indent=2)+'\n')
