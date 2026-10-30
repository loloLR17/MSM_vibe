"""Offline audit of existing evidence only. Never qualifies a physical hour."""
from pathlib import Path
from decimal import Decimal
import argparse,datetime,hashlib,json,re

def stamp(s):
    d=datetime.datetime.fromisoformat(s.replace('Z','+00:00'))
    return Decimal(str(d.hour*3600+d.minute*60+d.second))+Decimal(d.microsecond)/1000000

def audit(p):
    launch=json.loads((p/'HOUR-LAUNCH.json').read_text());terminal=json.loads((p/'HOUR-TERMINAL.json').read_text())
    wall=stamp(terminal['utc'])-stamp(launch['utc']);nominal=Decimal(terminal['result']['duration_us'])/1000000
    events=json.loads((p/'host-clock-events.json').read_bytes().decode('ascii','replace'));changes=[]
    for row in events:
        values=re.findall(r'2026-10-30T\d\d:\d\d:\d\d\.\d+Z',row['Message'].replace('?',''))
        assert len(values)>=2
        def exact(s):
            h,m,sec=s[11:-1].split(':');return Decimal(int(h)*3600+int(m)*60)+Decimal(sec)
        delta=exact(values[1])-exact(values[0]);changes.append(dict(old=values[0],new=values[1],step_seconds=str(delta)))
    step=sum((Decimal(v['step_seconds']) for v in changes),Decimal(0));corrected=wall-step
    files=['run_hour.py','run_capture.py','hardware_second.py','HOUR-LAUNCH.json','HOUR-TERMINAL.json','hardware/physical-hour.gdb','hardware/physical-hour.log','hardware/physical-hour-server.log','hardware/physical-hour-invocation.json','host-clock-events.json','physical-hour/result.json','physical-hour/metrics.json','physical-hour/header.bin','physical-hour/durable-footer.bin','artifact.json']
    hashes=[dict(path=f,bytes=(p/f).stat().st_size,sha256=hashlib.sha256((p/f).read_bytes()).hexdigest()) for f in files]
    return dict(scope='Existing evidence only; no hardware, clock change or replay',host_launch_UTC=launch['utc'],host_return_UTC=terminal['utc'],host_UTC_outer_interval_s=str(wall),MCU_nominal_duration_s=str(nominal),difference_s=str(nominal-wall),windows_steps=changes,windows_step_sum_s=str(step),conditional_outer_interval_after_discrete_Windows_steps_s=str(corrected),remaining_difference_s=str(nominal-corrected),conditional_minimum_clock_ratio=str(nominal/corrected),conditional_minimum_core_MHz=str(Decimal(160)*nominal/corrected),conditional_model_assumptions=['WSL follows these same discrete Windows steps exactly','No additional slewing, clock jumps or virtualization effects','Zero start/stop/transport overhead (optimistic for capture)'],conditional_values_are_not_proven_duration_bounds=True,actual_capture_duration_external_bounds=None,monotonic_START_STOP_pair_found=False,MCU_clock='MSI default calibration -> PLL nominal160MHz; DWT and SysTick same source',conclusion='Cannot retroactively qualify3600physical seconds; integrity qualification unchanged',provenance=hashes)
if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('evidence',type=Path);args=ap.parse_args();print(json.dumps(audit(args.evidence),indent=2))
