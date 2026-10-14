"""Generate a separate diagnostic overlay; never modify the pinned checkout."""
from pathlib import Path
import argparse, hashlib, json, re, difflib, subprocess
p=argparse.ArgumentParser();p.add_argument('--reference',required=True);p.add_argument('--out',required=True)
a=p.parse_args();src=Path(a.reference);out=Path(a.out);out.mkdir(parents=True,exist_ok=True)
head=subprocess.check_output(['git','rev-parse','HEAD'],cwd=src,text=True).strip()
assert head=='1e3c9183bad023ebf7d234134823d9270a0e8b64'
assert not subprocess.check_output(['git','status','--porcelain=v1'],cwd=src)
metadata=[]
def save(name,original,generated):
    (out/Path(name).name).write_text(generated)
    (out/(Path(name).name+'.diff')).write_text(''.join(difflib.unified_diff(
        original.splitlines(True),generated.splitlines(True),fromfile='upstream/'+name,tofile='overlay/'+name,n=2)))
    metadata.append(dict(source=name,source_sha256=hashlib.sha256((src/name).read_bytes()).hexdigest(),
        overlay_sha256=hashlib.sha256(generated.encode()).hexdigest()))
name='Core/Src/main.c';original=(src/name).read_text()
s=original.replace('#include "tusb.h"','#include "trace.h"')
start=s.index('int main(void)');stop=s.index('/**\n  * @brief System Clock Configuration',start)
body='''int main(void)
{
  HAL_Init();
  SystemPower_Config();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ICACHE_Init();
  ff_prepare();
  while (1) {
    if (ff_result.stage == FF_WAIT) ff_run();
    __NOP();
  }
}

'''
save(name,original,s[:start]+body+s[stop:])
name='Core/Src/stm32u5xx_it.c';original=(src/name).read_text()
s=original.replace('#include "tusb.h"','/* USB stack deliberately absent from this read-only probe. */')
assert s.count('tud_int_handler(0);')==1
s=s.replace('tud_int_handler(0);','/* No USB ISR work: OTG IRQ remains disabled. */')
save(name,original,s)
for name in ['Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_sd.c',
             'Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_sdmmc.c']:
    original=(src/name).read_text();s=original
    # Each hook is on a phase boundary or a taken error branch, never in FIFO polling.
    if name.endswith('hal_sd.c'):
        s=re.sub(r'(?m)^([ \t]*)(__HAL_SD_CLEAR_FLAG\(hsd,.*?;)$',
            lambda m:m[1]+'ff_data_preclear(hsd, __LINE__); /* FF_INSTRUMENT */\n'+m[0],s)
        # Software timeout branches must be captured before any flag cleanup.
        pattern=r'(if \(\(\(HAL_GetTick\(\) - tickstart\) >=  Timeout\) \|\| \(Timeout == 0U\)\)\n\s*\{)'
        s,count=re.subn(pattern,r'\1\n        ff_capture(hsd->Instance, HAL_SD_ERROR_TIMEOUT, __LINE__, 4U); /* FF_INSTRUMENT */',s)
        assert count>=2
        s=re.sub(r'(?m)^([ \t]*)(return HAL_SD_ERROR_(?!NONE)[A-Z_]+;)$',
            lambda m:m[1]+'ff_capture(hsd->Instance, '+m[2][7:-1]+', __LINE__, 4U); /* FF_INSTRUMENT */\n'+m[0],s)
        s=s.replace('hsd->ErrorCode = HAL_SD_ERROR_TIMEOUT;',
            'ff_capture(hsd->Instance, HAL_SD_ERROR_TIMEOUT, __LINE__, 4U); /* FF_INSTRUMENT */\n      hsd->ErrorCode = HAL_SD_ERROR_TIMEOUT;')
    else:
        for flag,code in [('CTIMEOUT','CMD_RSP_TIMEOUT'),('CCRCFAIL','CMD_CRC_FAIL')]:
            needle='__SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_FLAG_'+flag+');'
            assert needle in s
            s=s.replace(needle,'ff_capture(SDMMCx, SDMMC_ERROR_'+code+', __LINE__, 1U); /* FF_INSTRUMENT */\n    '+needle)
        # R1 logical errors/RESPCMD mismatch must be inspected before CMD flags clear.
        start=s.index('uint32_t SDMMC_GetCmdResp1(');stop=s.index('uint32_t SDMMC_GetCmdResp2(',start)
        section=s[start:stop]
        needle='__SDMMC_CLEAR_FLAG(SDMMCx, SDMMC_STATIC_CMD_FLAGS);'
        assert section.count(needle)==1
        section=section.replace(needle,'ff_r1_preclear(SDMMCx, SD_CMD, __LINE__); /* FF_INSTRUMENT */\n  '+needle)
        s=s[:start]+section+s[stop:]
        start=s.index('uint32_t SDMMC_GetCmdResp6(');stop=s.index('uint32_t SDMMC_GetCmdResp7(',start)
        section=s[start:stop];assert section.count(needle)==1
        section=section.replace(needle,'ff_r6_preclear(SDMMCx, __LINE__); /* FF_INSTRUMENT */\n  '+needle)
        s=s[:start]+section+s[stop:]
        # DMA linked-list utility functions have no peripheral pointer and are not used.
        stop=s.index('uint32_t SDMMC_DMALinkedList_BuildNode(')
        prefix=re.sub(r'(?m)^([ \t]*)(return SDMMC_ERROR_(?!NONE)[A-Z_]+;)$',
            lambda m:m[1]+'ff_capture(SDMMCx, '+m[2][7:-1]+', __LINE__, 1U); /* FF_INSTRUMENT */\n'+m[0],s[:stop])
        s=prefix+s[stop:]
    s='#include "trace.h" /* FF_INSTRUMENT */\n'+s
    # Verify that deleting only injected marker lines restores every original statement.
    recovered='\n'.join(line for line in s.splitlines() if '/* FF_INSTRUMENT */' not in line)+'\n'
    assert re.sub(r'\s+','',recovered)==re.sub(r'\s+','',original),name
    save(name,original,s)
(out/'overlay-provenance.json').write_text(json.dumps(dict(upstream=head,
    original_checkout_unchanged=True,hal_ll_original_statements_preserved=True,files=metadata),indent=2)+'\n')
