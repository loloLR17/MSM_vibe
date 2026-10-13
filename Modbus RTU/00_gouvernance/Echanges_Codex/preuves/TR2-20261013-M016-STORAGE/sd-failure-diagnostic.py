import hardware as h
elf=h.P/'qualification-build/tr2-special-storage_qualification-NON-CUMULATIVE.elf'
h.debug('sd-failure-state',elf,h.IDENTITY+"""p hsd2.State
p/x hsd2.ErrorCode
p sd
p store.recovery_required
p store.active_logical_bytes
p store.active_durable_bytes
p store.buffered_bytes
p store.writer
p/x hsd2.Instance->STA
p/x hsd2.Instance->CMD
p/x hsd2.Instance->RESP1
p/x hsd2.Instance->CLKCR
p (unsigned int)HAL_SD_GetCardState(&hsd2)
p/x hsd2.ErrorCode
p/x hsd2.Instance->RESP1
""")
print((h.P/'sd-failure-state.log').read_text())
