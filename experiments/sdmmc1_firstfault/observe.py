"""Qualified second-board operations, only after the required physical confirmations."""
from pathlib import Path
import argparse, subprocess, json, hashlib, time, struct, datetime
P=Path('/home/lolo/dev/msm/reports/TR2-SDMMC1-FIRSTFAULT-20261014')
SN='0032001F3235510C37333439'
SERVER='/mnt/c/ST/STM32CubeCLT_1.22.0/STLink-gdb-server/bin/ST-LINK_gdbserver.exe'
GDB='/mnt/c/ST/STM32CubeCLT_1.22.0/GNU-tools-for-STM32/bin/arm-none-eabi-gdb.exe'
CLI='/mnt/c/ST/STM32CubeCLT_1.22.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe'
UID='''x/3wx 0x0BFA0700
if *(unsigned int *)0x0BFA0700 != 0x001B0046 || *(unsigned int *)0x0BFA0704 != 0x38425018 || *(unsigned int *)0x0BFA0708 != 0x20333157
 quit 3
end
'''
def win(path):return subprocess.check_output(['wslpath','-w',str(path)],text=True).strip()
def debug(name,elf,body):
    script=P/(name+'.gdb');assert not script.exists(),name
    script.write_text('set pagination off\nset confirm off\nset print elements 0\nset remotetimeout 10\ntarget extended-remote localhost:61234\n'+UID+body+'\ndetach\nquit\n')
    command=[SERVER,'-d','-g','-i',SN,'--frequency','8000','-p','61234','-cp',r'C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin']
    with (P/(name+'-server.log')).open('wb') as out:
        server=subprocess.Popen(command,stdout=out,stderr=subprocess.STDOUT)
        try:
            time.sleep(2)
            gd=[GDB,'-nx','-q','-batch','-x',win(script),win(elf)]
            with (P/(name+'.log')).open('wb') as log:
                result=subprocess.run(gd,stdout=log,stderr=subprocess.STDOUT,timeout=45)
            (P/(name+'-invocation.json')).write_text(json.dumps(dict(server=command,gdb=gd,return_code=result.returncode,utc=datetime.datetime.now(datetime.timezone.utc).isoformat()),indent=2)+'\n')
            server.wait(timeout=15);result.check_returncode()
        finally:
            if server.poll() is None:server.terminate();server.wait(timeout=10)
def confirmation(name):
    # Files are written by the operator from actual user replies, never inferred.
    record=json.loads((P/(name+'.json')).read_text())
    assert record['confirmed'] is True and record['user_reply'],name
    return record
def main():
    parser=argparse.ArgumentParser();parser.add_argument('operation',choices=['flash-A','run-A','terminal-A','flash-B','run-B','terminal-B'])
    args=parser.parse_args();profile=args.operation[-1];width=1 if profile=='A' else 4
    if profile=='B':
        a=json.loads((P/'RUN-A-decoded.json').read_text())
        assert a['passed_bounded'] and a['profile']==1 and a['stage']==100 and a['run_count']==1 and a['ssr_width']==0
        assert a['initial_state']==4 and a['terminal_reason']==0 and not a['first_error']['valid']
    build=Path('/home/lolo/dev/msm/reports/SDMMC1-firstfault-'+profile+'-build')
    elf=build/'sdmmc1_firstfault.elf';image=build/'sdmmc1_firstfault.bin'
    artifacts=json.loads((P/('artifacts-'+profile+'.json')).read_text())
    assert hashlib.sha256(image.read_bytes()).hexdigest()==artifacts['bin_sha256']
    assert hashlib.sha256(elf.read_bytes()).hexdigest()==artifacts['elf_sha256']
    if args.operation.startswith('flash-'):
        confirmation('physical-preflash-A') if profile=='A' else None
        original=Path('/home/lolo/dev/msm/reports/SDMMC1-PoC-release-build/sdmmc1_poc.elf') if profile=='A' else Path('/home/lolo/dev/msm/reports/SDMMC1-firstfault-A-build/sdmmc1_firstfault.elf')
        previous_bytes=31437 if profile=='A' else 21852
        previous_hash='2a2fe744ab926789ae180b74f2a250b2a9202329b5afde51a1155cdd69d54c1a' if profile=='A' else json.loads((P/'artifacts-A.json').read_text())['bin_sha256']
        dump=P/('original-preflash-'+profile+'.bin')
        debug('original-preflash-'+profile,original,'dump binary memory '+win(dump)+' 0x08000000 '+hex(0x08000000+previous_bytes)+'\n'+ ('p ff_result\nif ff_result.stage != 100 || ff_result.run_count != 1 || ff_result.profile != 1\n quit 4\nend\n' if profile=='B' else ''))
        assert hashlib.sha256(dump.read_bytes()).hexdigest()==previous_hash,'Unexpected prior image: preserve and stop'
        command=[CLI,'-c','port=SWD','sn='+SN,'freq=8000','-w',win(image),'0x08000000','-v','-rst']
        with (P/('flash-'+profile+'-command.json')).open('x') as f:json.dump({'command':command,'bin_sha256':artifacts['bin_sha256']},f,indent=2)
        result=subprocess.run(command,capture_output=True,timeout=55)
        (P/('flash-'+profile+'.log')).write_bytes(result.stdout+result.stderr);result.check_returncode()
        assert b'Download verified successfully' in result.stdout
        debug('flash-'+profile+'-readback-wait',elf,'dump binary memory '+win(P/('flash-'+profile+'-readback.bin'))+' 0x08000000 '+hex(0x08000000+len(image.read_bytes()))+'''\np ff_result
if ff_result.magic != 0x46303133 || ff_result.profile != WIDTH || ff_result.stage != 1 || ff_result.run_count != 0
 quit 4
end
'''.replace('WIDTH',str(width)))
        assert (P/('flash-'+profile+'-readback.bin')).read_bytes()==image.read_bytes()
    elif args.operation.startswith('run-'):
        confirmation('physical-start-'+profile)
        # Creating the marker before trigger prevents operator retries after an interrupted call.
        with (P/('RUN-'+profile+'-started.json')).open('x') as f:json.dump({'utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'maximum_runs':1},f)
        debug('RUN-'+profile+'-trigger',elf,'''p ff_result
if ff_result.magic != 0x46303133 || ff_result.profile != WIDTH || ff_result.stage != 1 || ff_result.run_count != 0 || ff_result.trigger != 0
 quit 4
end
set ff_result.trigger = 0x52454144
'''.replace('WIDTH',str(width)))
        # No GDB connection/halt during the bounded polling interval.
        time.sleep(10)
        terminal(elf,profile,width)
    else:
        assert (P/('RUN-'+profile+'-started.json')).exists()
        terminal(elf,profile,width)
def terminal(elf,profile,width):
    # Unique output names prevent repeating a successful terminal collection.
    debug('RUN-'+profile+'-terminal',elf,'''p ff_result
p hsd1
'''+ 'dump binary memory '+win(P/('RUN-'+profile+'-result.bin'))+' &ff_result ((unsigned char *)&ff_result)+sizeof(ff_result)\n'+
        'dump binary memory '+win(P/('RUN-'+profile+'-data512.bin'))+' ff_data512 ff_data512+512\n'+
        'dump binary memory '+win(P/('RUN-'+profile+'-data8192.bin'))+' ff_data8192 ff_data8192+8192\n'+'''
if ff_result.stage != 99 && ff_result.stage != 100
 printf "OPERATOR_TIMEOUT_HALTED: no detach/resume, reset or retry\\n"
 disconnect
 quit 6
end
'''.replace('WIDTH',str(width)))
if __name__=='__main__':main()
