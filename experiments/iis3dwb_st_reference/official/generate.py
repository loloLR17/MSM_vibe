from pathlib import Path
import sys,hashlib,json,difflib
source=Path(sys.argv[1]);output=Path(sys.argv[2]);raw=source.read_bytes()
assert hashlib.sha256(raw).hexdigest()=="f8650edb8a1364624214ff0a3bc8132ec8f2d93a81c19b6a967769696acb34ec"
s=raw.decode().replace('\r\n','\n')
start=s.index('void iis3dwb_fifo(void)');end=s.index('/*\n * @brief  Write generic')
original_core=s[start:end]
old='"%d TIMESTAMP [ms] %d\\r\\n", k, *ts'
new='"%d TIMESTAMP [ms] %ld\\r\\n", k, (long)*ts'
assert original_core.count(old)==1
core=original_core.replace(old,new)
assert core.replace(new,old)==original_core
prefix=s[:start].replace('#include "iis3dwb_reg.h"','#include "iis3dwb_reg.h"\n#include "port.h"')
# This alias is applied only to the generated example, never the driver or original.
prefix=prefix.replace('fifo_data[FIFO_WATERMARK]','fifo_data[512]')
prefix+='\n#if OFFICIAL_STATUS10\n#define iis3dwb_fifo_status_get official_status10\n#endif\n'
tail="""
static int32_t platform_read(void *h,uint8_t r,uint8_t *d,uint16_t n){return official_read(h,r,d,n);}
static int32_t platform_write(void *h,uint8_t r,const uint8_t *d,uint16_t n){return official_write(h,r,d,n);}
static void tx_com(uint8_t *d,uint16_t n){official_console(d,n);}
static void platform_delay(uint32_t n){official_delay(n);}
static void platform_init(void){official_init();}
"""
ported=prefix+core+tail;output.parent.mkdir(parents=True,exist_ok=True);output.write_text(ported)
assert ported.count(core)==1
assert ported[ported.index('void iis3dwb_fifo(void)'):ported.index('void iis3dwb_fifo(void)')+len(core)]==core
output.with_suffix('.patch').write_text(''.join(difflib.unified_diff(s.splitlines(True),ported.splitlines(True),fromfile='ST-original',tofile='NUCLEO-platform-port')))
output.with_suffix('.json').write_text(json.dumps({'original_sha256':hashlib.sha256(raw).hexdigest(),'sensor_function_sha256':hashlib.sha256(core.encode()).hexdigest(),'sensor_control_sequence_unchanged':True,'format_only_patch':'timestamp %ld with long cast','buffer_only_patch':'fifo_data[256] -> [512], watermark unchanged','original_function_sha256':hashlib.sha256(original_core.encode()).hexdigest(),'generated_sha256':hashlib.sha256(ported.encode()).hexdigest()},indent=2)+'\n')
