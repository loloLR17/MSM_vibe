import hardware as h
elf=h.P/'qualification-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf'
h.debug('sd-resync-5mhz-readonly',elf,h.IDENTITY+"""set variable hsd2.Init.ClockDiv=16
p (unsigned int)HAL_SD_Init(&hsd2)
p/x hsd2.ErrorCode
p hsd2.CID
p (unsigned int)HAL_SD_GetCardState(&hsd2)
p (unsigned int)HAL_SD_ReadBlocks(&hsd2,tr2_commission_sd_prefix,0,16,2000)
p/x hsd2.ErrorCode
"""+'dump binary memory '+h.win(h.P/'sd-prefix-after-failure-resync.bin')+' tr2_commission_sd_prefix tr2_commission_sd_prefix+8192\n')
print((h.P/'sd-resync-5mhz-readonly.log').read_bytes().decode('utf-8',errors='backslashreplace'))
assert (h.P/'sd-prefix-after-failure-resync.bin').read_bytes()==(h.P/'current-sd-prefix.bin').read_bytes()
