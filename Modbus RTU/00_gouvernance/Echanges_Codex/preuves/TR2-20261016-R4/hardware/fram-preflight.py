from hardware import *
elf=Path('/home/lolo/dev/msm/reports/TR2-M016-storage-20261013T172331Z/qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf')
body=IDENTITY+'''frame function CommissioningReadOnly
p storage
p tr2_commission_fram_result
'''
for name in ['fram-before.bin','fram-verification-read.bin']:
 body+='set $r4_read_status=(unsigned int)stm32_fram_storage_read(&storage,0,tr2_commission_fram_backup,262144)\np $r4_read_status\nif $r4_read_status != 0\n quit 4\nend\n'
 body+='dump binary memory '+win(P/name)+' tr2_commission_fram_backup tr2_commission_fram_backup+262144\n'
body+='frame 0\nx/2wx 0xE000E100\n'
debug('fram-preflight-local-storage',elf,body,timeout=55)
a=(P/'fram-before.bin').read_bytes();b=(P/'fram-verification-read.bin').read_bytes();assert a==b and len(a)==262144
regions={}
for name,lo,hi in [('historical',0,122880),('LIVE',122880,245760),('tail',245760,262144)]:
 data=a[lo:hi];regions[name]={'start':lo,'end':hi,'sha256':hashlib.sha256(data).hexdigest(),'nonzero':sum(x!=0 for x in data),'not_ff':sum(x!=255 for x in data)}
summary={'sha256':hashlib.sha256(a).hexdigest(),'bytes':len(a),'two_reads_equal':True,'regions':regions,'writes':0}
(P/'fram-preflight-summary.json').write_text(json.dumps(summary,indent=2)+'\n');print(summary)
