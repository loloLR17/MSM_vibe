from hardware import *
elf=P.parent/'bench-build/tr2-r4-qualification-NON-PRODUCTION.elf'
debug('bench-readonly-spi-failure',elf,IDENTITY+'''p hspi1
p/x *(SPI_TypeDef *)0x40013000
p uwTick
p HAL_GetTick()
p/x *(unsigned int*)0xE000E010
x/64bx baseline
''')
