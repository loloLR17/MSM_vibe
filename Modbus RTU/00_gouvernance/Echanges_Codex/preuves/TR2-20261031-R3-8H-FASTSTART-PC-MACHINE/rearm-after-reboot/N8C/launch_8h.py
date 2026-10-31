"""Run only after the explicit user signal LANCE 8H. One persistent attempt lock.
Reuse qualified detached START; no build, flash, reset, gate, Git, or analysis.
"""
from pathlib import Path
import argparse,json,sys,subprocess,time,datetime,hashlib
import hardware_second as h
import start_detached as start
P=Path(__file__).parent

def main():
 a=argparse.ArgumentParser();g=a.add_mutually_exclusive_group(required=True);g.add_argument('--signal',choices=['LANCE 8H']);g.add_argument('--check-only',action='store_true');args=a.parse_args()
 # Even a failed/uncertain attempt remains latched: inspect, never auto-retry.
 if not args.check_only:
  with (P/'LONG_START_ATTEMPT.json').open('x') as f:
   json.dump(dict(signal='LANCE 8H',utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),f)
 begin=time.monotonic();ready=json.loads((P/'READY.json').read_text());assert ready['prepared'] and ready['long_never_started']
 art=json.loads((P/'artifact.json').read_text());image=(P/'arm/r3_coex.bin').read_bytes();assert hashlib.sha256(image).hexdigest()==ready['bin_sha256']
 # Re-read native power, COM5 parent and AC standby/hibernate; no policy writes.
 subprocess.run([sys.executable,str(P/'read_host.py')],check=True,timeout=55)
 import re
 for n in ['sleep','hibernate']:
  values=re.findall(rb'0x([0-9a-fA-F]{8})',(P/(n+'-policy.txt')).read_bytes());assert len(values)>=5 and int(values[-2],16)==0,(n,values)
 # Enforce exclusive port availability before observer starts. No UART TX here.
 ps="""$ErrorActionPreference='Stop';$s=[IO.Ports.SerialPort]::new('COM5',115200,[IO.Ports.Parity]::None,8,[IO.Ports.StopBits]::One);try{$s.Open()}finally{if($s.IsOpen){$s.Close()};$s.Dispose()}"""
 subprocess.run([start.PS,'-NoProfile','-NonInteractive','-Command',ps],check=True,timeout=15)
 readback=P/('hardware/prepare-final-image-readback.bin' if args.check_only else 'hardware/launch-image-readback.bin');path=h.win(readback).replace('\\','/')
 body=h.IDENTITY+f'''p r3_result
p r3_card
p durable_status
if r3_result.stage!=4 || r3_result.run!=3 || r3_result.error || r3_clock_fault || r3_command || fifo.running || r3_writer.active || q.count || witness_errors || !(*(unsigned int*)0x42021414 &0x1000)
 quit 4
end
if !r3_card.initialized || r3_card.failed || r3_card.sectors!=62333952 || !durable_status.mounted || durable_status.fs_type!=3 || durable_status.error || durable_status.io_uncertain
 quit 5
end
set $cid_rc = (int)'sd_spi.c'::reg_data(&r3_card,10,(unsigned char*)(r3_header+128))
p $cid_rc
if $cid_rc || ((unsigned char*)r3_header)[128]!=0x03 || ((unsigned char*)r3_header)[129]!=0x53 || ((unsigned char*)r3_header)[130]!=0x44 || ((unsigned char*)r3_header)[131]!=0x53 || ((unsigned char*)r3_header)[132]!=0x44 || ((unsigned char*)r3_header)[133]!=0x33 || ((unsigned char*)r3_header)[134]!=0x32 || ((unsigned char*)r3_header)[135]!=0x47 || ((unsigned char*)r3_header)[136]!=0x85 || ((unsigned char*)r3_header)[137]!=0x8b || ((unsigned char*)r3_header)[138]!=0xf0 || ((unsigned char*)r3_header)[139]!=0x92 || ((unsigned char*)r3_header)[140]!=0x3c || ((unsigned char*)r3_header)[141]!=0x01 || ((unsigned char*)r3_header)[142]!=0x92 || ((unsigned char*)r3_header)[143]!=0x7b
 quit 6
end
dump binary memory {path} 0x08000000 0x{0x08000000+len(image):x}
set {{char[15]}} r3_header = "0:/N8C0005.TR2"
set $rc = (int)f_open(&'fat_volume.c'::file,(char*)r3_header,1)
p $rc
if $rc != 4
 if $rc == 0
  call (int)f_close(&'fat_volume.c'::file)
 end
 quit 7
end
set {{char[3]}} r3_header = "0:"
set $fr = (int)f_getfree((char*)r3_header,(DWORD*)(r3_header+64),(FATFS**)(r3_header+72))
p $fr
p *(unsigned int*)(r3_header+64)
if $fr || *(unsigned int*)(r3_header+64)<20480
 quit 8
end
'''
 h.debug('prepare-final-preflight' if args.check_only else 'launch-minimal-preflight',P/'arm/r3_coex.elf',body,timeout=55)
 assert readback.read_bytes()==image
 if args.check_only:
  print('READONLY PREPARATION PREFLIGHT PASS; no START submitted',flush=True);return
 # Existing firmware re-reads and hashes the short gate before allocation/START.
 start.start('n8c-long',3,5,28801)
 elapsed=time.monotonic()-begin
 (P/'START_LATENCY.json').write_text(json.dumps(dict(seconds_until_observed_ACTIVE=elapsed,includes_preflight_and_observer=True,utc_annotation=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
 print('START confirmed; no persistent Codex/WSL/GDB monitor; do not disconnect power',flush=True)
if __name__=='__main__':main()
