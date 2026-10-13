import hardware as h
elf=h.P/'qualification-attempt02-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf'
body=h.IDENTITY+"""frame function stm32_storage_qualification_run
p fram
set $m016_fram_status=(unsigned int)stm32_fram_storage_read(fram,0,tr2_commission_fram_backup,262144)
p $m016_fram_status
if $m016_fram_status != 0
 quit 5
end
"""
body+='dump binary memory '+h.win(h.P/'fram-after-attempts.bin')+' tr2_commission_fram_backup tr2_commission_fram_backup+262144\n'
body+="""frame 0
set variable hsd2.Init.ClockDiv=201
p (unsigned int)HAL_SD_Init(&hsd2)
p/x hsd2.Instance->CLKCR
p (unsigned int)HAL_SD_GetCardState(&hsd2)
set $m016_mbr_status=(unsigned int)HAL_SD_ReadBlocks(&hsd2,tr2_commission_sd_prefix,0,16,2000)
p $m016_mbr_status
if $m016_mbr_status != 0
 quit 6
end
p/x hsd2.ErrorCode
"""
body+='dump binary memory '+h.win(h.P/'mbr-prefix-after-attempts.bin')+' tr2_commission_sd_prefix tr2_commission_sd_prefix+8192\n'
body+="""p (unsigned int)campaign_data_store_bulk_init(&store,&region.interface,payload,16384,scratch,16420)
p (unsigned int)recover_saved()
p storage_qual_recovery_status
p storage_qual_recovery_bytes
"""
body+='dump binary memory '+h.win(h.P/'attempt02-recovery-status.bin')+' storage_qual_recovery_status storage_qual_recovery_status+2\n'
body+='dump binary memory '+h.win(h.P/'attempt02-recovery-bytes.bin')+' storage_qual_recovery_bytes storage_qual_recovery_bytes+2\n'
body+="""p (unsigned int)campaign_bulk_media_region_init(&region,&raw,8388608,8388608)
p (unsigned int)campaign_data_store_bulk_init(&store,&region.interface,payload,16384,scratch,16420)
p (unsigned int)store.interface.recover_campaign(&store,0x01600001,(CampaignDataRecoveryResult *)inspect)
p *(CampaignDataRecoveryResult *)inspect
"""
h.debug('attempt02-prefix-recovery-readonly',elf,body,timeout=60)
print((h.P/'attempt02-prefix-recovery-readonly.log').read_bytes().decode('utf-8',errors='backslashreplace'))
