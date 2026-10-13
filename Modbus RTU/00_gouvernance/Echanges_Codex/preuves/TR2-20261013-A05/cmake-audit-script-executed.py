from pathlib import Path
import subprocess,json,concurrent.futures
r=Path.cwd();p=Path(Path('/tmp/tr2-a05-proof').read_text());src=r/'Modbus RTU/05_Firmware/platform/stm32'
base=['cmake','-G','Ninja','-S',str(src),'-DCMAKE_TOOLCHAIN_FILE='+str(src/'cmake/arm-none-eabi-gcc.cmake'),'-DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0']
auth=['-DTR2_COMMISSIONING_MANIFEST='+str(r/'Modbus RTU/00_gouvernance/COMMISSIONING_M014_H.json'),'-DTR2_COMMISSIONING_REGISTRY='+str(r/'Modbus RTU/00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json')]
cases=[('default',[],0,'accepted harness selection; not safe production'),('qualification-U',['-DTR2_STM32_ST_FIFO_WINDOW=ON'],0,'accepted isolated qualification; not a production image'),('production-no-TX',['-DTR2_STM32_PRODUCTION=ON'],1,'Production requires qualified RS485'),('cooperative-no-production',['-DTR2_STM32_FIFO_COOPERATIVE=ON'],1,'Cooperative FIFO requires'),('cooperative-no-observer',['-DTR2_STM32_FIFO_COOPERATIVE=ON','-DTR2_STM32_PRODUCTION=ON','-DTR2_STM32_RS485_TX_QUALIFIED=ON'],1,'Cooperative FIFO requires'),('production-manufacturing',['-DTR2_STM32_PRODUCTION=ON','-DTR2_STM32_MANUFACTURING=ON','-DTR2_STM32_RS485_TX_QUALIFIED=ON'],1,'are exclusive'),('production-probe',['-DTR2_STM32_PRODUCTION=ON','-DTR2_STM32_COMMISSIONING_READONLY=ON','-DTR2_STM32_RS485_TX_QUALIFIED=ON'],1,'are exclusive'),('cooperative-U',['-DTR2_STM32_FIFO_COOPERATIVE=ON','-DTR2_STM32_PRODUCTION=ON','-DTR2_STM32_RS485_TX_QUALIFIED=ON','-DTR2_STM32_IRQ_DIAGNOSTICS=ON','-DTR2_STM32_ST_FIFO_WINDOW=ON'],1,'Cooperative FIFO requires'),('cooperative-silence',['-DTR2_STM32_FIFO_COOPERATIVE=ON','-DTR2_STM32_PRODUCTION=ON','-DTR2_STM32_RS485_TX_QUALIFIED=ON','-DTR2_STM32_IRQ_DIAGNOSTICS=ON','-DTR2_STM32_PRODUCTION_SILENCE_TEST=ON'],1,'Cooperative FIFO requires'),('SPI16',['-DTR2_STM32_FIFO_SPI_MHZ=16'],1,'bounded to4/8MHz')]
def run(case):
 name,args,expected,message=case;cmd=base+['-B',str(p/('configure-'+name))]+args+auth
 z=subprocess.run(cmd,capture_output=True);raw=z.stdout+z.stderr;(p/(name+'-configure.log')).write_bytes(raw)
 # Specific guard, not a generic missing SDK/compiler failure.
 actual_reject=z.returncode!=0
 assert actual_reject==bool(expected),(name,z.returncode)
 if expected:assert message.encode() in raw,name
 return {'case':name,'command':cmd,'return_code':z.returncode,'current_behavior':'REFUSED by expected guard' if expected else 'ACCEPTED configuration only','guard_text_expected':message,'build_executed':False,'physical_executed':False,'future_safe_default_not_demonstrated':name=='default'}
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:results=list(pool.map(run,cases))
(p/'cmake-current-characterization.json').write_text(json.dumps(results,indent=2)+'\n');(p/'cmake-audit-script-executed.py').write_bytes(Path(__file__).read_bytes())
print('10 configurations characterized:2 accepted,8 guard refusals; no build, flash or test-source changes.')
