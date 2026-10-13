import hardware as h
elf=h.P/'qualification-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf'
h.debug('sd-5mhz-prefix-recovery-readonly',elf,h.IDENTITY+"""p (unsigned int)HAL_SD_ConfigWideBusOperation(&hsd2,0)
p/x hsd2.Instance->CLKCR
p/x hsd2.ErrorCode
p (unsigned int)HAL_SD_GetCardState(&hsd2)
p (unsigned int)campaign_data_store_bulk_init(&store,&region.interface,payload,4096,scratch,4132)
p (unsigned int)recover_saved()
p storage_qual_recovery_status
p storage_qual_recovery_bytes
p store.recovery_required
""")
print((h.P/'sd-5mhz-prefix-recovery-readonly.log').read_bytes().decode('utf-8',errors='backslashreplace'))
