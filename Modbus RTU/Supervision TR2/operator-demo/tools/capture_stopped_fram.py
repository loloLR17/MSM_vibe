"""Qualified c36c971 TR2-001 FRAM audit. Requires mission debug permission and
observation service closed after a terminal STOP. No flash/reset/FRAM write.
A successful capture is NOT a post-detach COM4 health check; perform that next.
"""
from pathlib import Path
import argparse, datetime, hashlib, json, subprocess, time

SN = '003500463235510B37333439'
ELF_SHA = 'ba999ab4a8aa674a977ca8e3ab95469eba46c0d12ff2cdbf98c89bcfb1a1cbe3'
BIN_SHA = '9bbe397911185b798a0b030aba893a325bfdf6afa6947145bda9157f3bff0981'
CLT = Path('/mnt/c/ST/STM32CubeCLT_1.22.0')

def win(path):
    return subprocess.check_output(['wslpath', '-w', str(path)], text=True).strip()

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--elf', type=Path, required=True)
    ap.add_argument('--evidence', type=Path, required=True)
    ap.add_argument('--stop-proof', type=Path, required=True)
    args = ap.parse_args()
    if hashlib.sha256(args.elf.read_bytes()).hexdigest() != ELF_SHA:
        ap.error('Only the qualified c36c971 operator ELF is supported')
    terminals = [x for line in args.stop_proof.read_text().splitlines()
                 if line.startswith('{') and (x := json.loads(line)).get('state') == 'terminal_and_effect_observed'
                 and x.get('command') == 'stop']
    if len(terminals) != 1:
        ap.error('Exactly one native terminal STOP proof is required')
    stop = terminals[0]; b5 = stop['b5']; b3 = stop['b3']
    if not (len(b5) == 29 and len(b3) == 48 and b5[10] == b5[16] == 4 and b5[17] == 0
            and b5[14] == 14 and b5[15] == stop['transaction'] and not b5[13] & 2
            and (b5[26] << 16 | b5[27]) == stop['epoch']
            and b3[0] == 1 and b3[1] & 512 and not b3[1] & 4):
        ap.error('Terminal, identity or stopped/HELD effect missing')
    out = args.evidence.resolve(); out.mkdir(parents=True, exist_ok=False)
    # Refuse before connecting GDB when a Windows observer or debugger owns the bench.
    ps = r"""[Console]::OutputEncoding=New-Object System.Text.UTF8Encoding($false)
$ErrorActionPreference='Stop'
$p=@(Get-CimInstance Win32_Process | Where-Object {$_.Name -in @('TR2.Supervision.Service.exe','ST-LINK_gdbserver.exe','arm-none-eabi-gdb.exe')} | Select-Object Name,ProcessId,CommandLine)
[pscustomobject]@{Utc=[datetime]::UtcNow.ToString('o');Processes=$p} | ConvertTo-Json -Depth 4
"""
    with (out/'windows-owners.json').open('wb') as log:
        subprocess.run(['/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe',
                        '-NoProfile', '-Command', ps], stdout=log, stderr=subprocess.STDOUT,
                       timeout=20, check=True)
    if json.loads((out/'windows-owners.json').read_bytes().decode('utf-8-sig'))['Processes']:
        raise RuntimeError('Observer/debugger still running; refused before any target connection')
    def guard(condition):
        return f'if {condition}\n echo UNSAFE_AUDIT_REFUSED\\n\n detach\n quit 7\nend\n'
    identity = guard('*(unsigned int*)0x0BFA0700 != 0x004A003D || *(unsigned int*)0x0BFA0704 != 0x38425018 || *(unsigned int*)0x0BFA0708 != 0x20333157')
    stopped = guard('owner.acquisition.fifo.running || engine.has_active || r4u_result.error != 0 || engine.state != LIVE_READY || engine.last.status != 4 || engine.last.result != 0 || engine.last.request.command.identity.command_code != 14 || engine.last.request.epoch != '+str(stop['epoch'])+' || engine.last.request.command.transaction_id != '+str(stop['transaction']))
    def debug(name, body):
        script = out/(name+'.gdb')
        script.write_text('set pagination off\nset confirm off\nset remotetimeout 10\ntarget extended-remote localhost:61234\n'+identity+stopped+body+'\ndetach\nquit\n')
        server = [str(CLT/'STLink-gdb-server/bin/ST-LINK_gdbserver.exe'), '-d', '-g', '-i', SN,
                  '--frequency', '8000', '-p', '61234', '-cp', r'C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin']
        gdb = [str(CLT/'GNU-tools-for-STM32/bin/arm-none-eabi-gdb.exe'), '-nx', '-q', '-batch', '-x', win(script), win(args.elf.resolve())]
        (out/(name+'-commands.json')).write_text(json.dumps(dict(server=server,gdb=gdb),indent=2)+'\n')
        with (out/(name+'-server.log')).open('wb') as log:
            proc = subprocess.Popen(server, stdout=log, stderr=subprocess.STDOUT)
            try:
                time.sleep(2)
                with (out/(name+'.log')).open('wb') as evidence:
                    result = subprocess.run(gdb, stdout=evidence, stderr=subprocess.STDOUT, timeout=35)
                proc.wait(timeout=15)
                (out/(name+'-exit.json')).write_text(json.dumps(dict(exit=result.returncode,utc=datetime.datetime.now(datetime.timezone.utc).isoformat())))
                result.check_returncode()
            finally:
                if proc.poll() is None:
                    proc.terminate(); proc.wait(timeout=10)
    # First session: debugger reads only, no target C function. Verify Flash bytes
    # before trusting the ELF's guards and before any target function invocation.
    debug('image-readonly', 'p r4u_result\np engine.last\ndump binary memory '+win(out/'flash-prefix.bin')+' 0x08000000 0x08023E50\n')
    if hashlib.sha256((out/'flash-prefix.bin').read_bytes()).hexdigest() != BIN_SHA:
        raise RuntimeError('Installed Flash differs; no FRAM function was invoked')
    # A random interrupted service has a live elapsed-time scope. An injected
    # 0.85s target call there correctly trips its 2.5ms deadline on continuation.
    # At entry to the next step there is no enclosing time measurement; the
    # normal enter resets seen. Never clear a guard/counter to hide the delay.
    idle = guard('work.depth != 0 || work.servicing || runtime.main_busy || runtime.servicing')
    debug('idle-boundary-fram', 'tbreak live_runtime_step\ncontinue\n'+stopped+idle+'''bt
p work
p r4u_result
p engine.last
p store.generation
p live.generation
p owner.acquisition.fifo.error
p owner.acquisition.fifo.stop_error
set $rr=(unsigned int)stm32_fram_storage_read(&fram,0,baseline,262144)
p $rr
'''+guard('$rr != 0')+'dump binary memory '+win(out/'fram.bin')+''' baseline baseline+262144
p work
''')
    raw = (out/'fram.bin').read_bytes()
    if len(raw) != 262144:
        raise RuntimeError('Incomplete FRAM dump')
    result = dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),target=SN,
                  elf_sha256=ELF_SHA,flash_prefix_sha256=BIN_SHA,fram_sha256=hashlib.sha256(raw).hexdigest(),
                  epoch=stop['epoch'],stop_transaction=stop['transaction'],idle_scope_guard=True,
                  flash_reset_persistent_write=False,deadline_guards_modified=False,
                  post_detach_COM4_health_verified=False,
                  next_action='Start observation service and prove fresh B0/B3 after detach before claiming ready')
    (out/'capture.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))

if __name__ == '__main__':
    main()
