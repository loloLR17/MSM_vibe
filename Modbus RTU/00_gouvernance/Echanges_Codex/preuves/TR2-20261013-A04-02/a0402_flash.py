from pathlib import Path
import sys,hashlib,json,time
sys.path.insert(0,'/tmp');import a0402_hardware as h
image=h.P/'build-artifacts/tr2-cumulative-m014-m015-spi4.bin';elf=image.with_suffix('.elf');assert len(image.read_bytes())==169948 and hashlib.sha256(image.read_bytes()).hexdigest()=='b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b'
profile=Path('/home/lolo/dev/msm/projects/MSM_vibe-A04-02/Modbus RTU/05_Firmware/build-stm32-cumulative/build-profile.txt').read_text()
for flag in ['PRODUCTION','FIFO_COOPERATIVE','IRQ_DIAGNOSTICS','RS485_TX_QUALIFIED']:assert 'TR2_STM32_'+flag+'=ON\n' in profile
assert 'TR2_STM32_LEGACY_SD_HARNESS=OFF\n' in profile and 'TR2_STM32_FIFO_SPI_MHZ=4\n' in profile
prefix=h.win(h.P).replace('\\','/');h.flash('flash-a0402',image,'8bcb843')
h.debug('flash-readback-reset',elf,h.IDENTITY+f'dump binary memory {prefix}/flash-after.bin 0x08000000 0x08200000\nmonitor reset\n')
new=(h.P/'flash-after.bin').read_bytes();old=(h.P/'flash-before.bin').read_bytes();span=((image.stat().st_size+8191)//8192)*8192;assert new[:image.stat().st_size]==image.read_bytes() and new[span:]==old[span:]
(h.P/'flash-verification.json').write_text(json.dumps(dict(profile='CUMULATIVE SPI4 Debug',bin_sha256=hashlib.sha256(image.read_bytes()).hexdigest(),programming_verified=True,gdb_flash_readback_exact=True,unchanged_tail_after_page_span=span,tail_sha256=hashlib.sha256(new[span:]).hexdigest(),software_reset_after_programmer=True,reason='Qualified GDB reset after programmer exit restores the NVIC state documented in M013-D; no HOTPLUG read'),indent=2)+'\n')
time.sleep(3)
body=h.IDENTITY+'''p SystemCoreClock
p tr2_production_stage
p tr2_production_result
p production_runtime.recovery
p cooperative
p tr2_fifo_fault
p bench_ready
p cooperative_fram_state
p cooperative_fram_crc_before
p g_serial
x/3wx 0xE000E100
x/2wx 0xE000ED28
if tr2_production_stage != 2 || tr2_production_result != 0 || production_runtime.recovery.generation != 12 || bench_ready != 1 || cooperative.state != 1 || cooperative.who != 123 || tr2_fifo_fault != 0 || cooperative_fram_state != 1
 quit 4
end
if g_serial.tx_busy || g_serial.tx_fault || g_serial.uart.ErrorCode || g_serial.event_head != g_serial.event_tail || tr2_irq_observer.events_dropped != 0
 quit 5
end
if SystemCoreClock != 160000000 || (*(unsigned int *)0xE000E104 & 0x20000)==0 || (*(unsigned int *)0xE000E108 & 4)==0
 quit 6
end
'''+f'dump binary memory {prefix}/fram-before-capture.bin official_words (official_words+262144)\ndump binary memory {prefix}/persistent-before-capture.bin production_runtime.candidate (production_runtime.candidate+51914)\n'
h.debug('new-firmware-preflight',elf,body)
assert (h.P/'persistent-before-capture.bin').read_bytes()==(h.P/'persistent-before.bin').read_bytes()
(h.P/'fram-preservation-before.json').write_text(json.dumps(dict(bytes=(h.P/'fram-before-capture.bin').stat().st_size,sha256=hashlib.sha256((h.P/'fram-before-capture.bin').read_bytes()).hexdigest(),persistent_image_unchanged=True,generation=12),indent=2)+'\n')
print('New cumulative ready/ARMED, IRQs enabled, gen12 unchanged; no serial transmission yet.')
