from hardware import *
elf=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
assert elf.is_file()
body=IDENTITY+'''info registers pc sp xpsr
x/2wx 0xE000E100
x/16wx 0x40013000
x/12wx 0x46002000
p hspi1.Init
p SystemCoreClock
p/x *(RCC_TypeDef*)0x46020C00
'''
for name in ['flash-before.bin','flash-verification-read.bin']:
 body+='dump binary memory '+win(P/name)+' 0x08000000 0x08200000\n'
debug('preflight-readonly',elf,body,timeout=55)
a=(P/'flash-before.bin').read_bytes();b=(P/'flash-verification-read.bin').read_bytes();assert a==b and len(a)==2097152
baseline=elf.with_suffix('.bin').read_bytes()
summary={'flash_sha256':hashlib.sha256(a).hexdigest(),'bytes':len(a),'two_reads_equal':True,'known_attempt02_bytes':len(baseline),'known_attempt02_sha256':hashlib.sha256(baseline).hexdigest(),'known_attempt02_prefix_equal':a[:len(baseline)]==baseline,'operations':['GDB attach/halt','read UID/registers/Flash','detach/resume'],'no_reset':True}
(P/'preflight-summary.json').write_text(json.dumps(summary,indent=2)+'\n');print(summary)
