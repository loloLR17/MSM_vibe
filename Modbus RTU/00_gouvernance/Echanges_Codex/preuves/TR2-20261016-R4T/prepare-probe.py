from pathlib import Path
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4T-monotonic-recovery');fw=r/'Modbus RTU/05_Firmware';old=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4S-service-budget-safe-recovery/Modbus RTU/05_Firmware')
# Reuse inspected calibration and bounded trace support; preserve readonly entry.
f=fw/'platform/stm32/stm32_r4s_bench.c';a=(old/'platform/stm32/stm32_r4s_bench.c').read_text();f.write_text(a)
a=(old/'platform/stm32/stm32_r4s_recovery.c').read_text().replace('r4s_recovery','r4t_recovery').replace('"r4s_maintenance.h"','"r4t_maintenance.h"')
a=a.replace('static uint8_t saved_intent[128];','static uint8_t saved_intent[128];\nstatic R4tRetirement retirement;')
x=a.index(' REQUIRE(r4m_isolate_journal');y=a.index(' REQUIRE(r4m_open_after_renew',x)
a=a[:x]+''' REQUIRE(r4t_retire_and_admit(&retirement,&engine));
 r4m_guard=retirement.gate;
 REQUIRE(compare_protected());
 r4_result.stage=41;r4t_recovery.stage=3;
 while(engine.has_active){REQUIRE(live_command_engine_step(&engine));}
'''+a[y:]
a=a.replace('REQUIRE(r4s_capture(&r4m_guard,&engine));','REQUIRE(r4t_capture(&retirement,&engine,baseline));r4m_guard=retirement.gate;')
a=a.replace('Host verifies full FRAM equals model-renew','Host verifies full FRAM equals model-renew')
a=a.replace('"r4s_maintenance.h"','"r4t_maintenance.h"')
(fw/'platform/stm32/stm32_r4t_recovery.c').write_text(a)
f=fw/'platform/stm32/CMakeLists.txt';a=f.read_text();pos=a.index('option(TR2_STM32_R4S_BENCH');a=a[:pos]+'''option(TR2_STM32_R4T_RECOVERY "R4T exact monotone retirement and physical B3; NON-PRODUCTION" OFF)
if(TR2_STM32_R4T_RECOVERY AND NOT TR2_STM32_R4S_BENCH)
 message(FATAL_ERROR "R4T requires explicit R4S/R4R hardware recipe")
endif()
'''+a[pos:];pos=a.index('\n',a.index(' set(TR2_ARTIFACT_NAME "tr2-r4s-readonly-service-NON-PRODUCTION")'));a=a[:pos]+a[pos:];pos=a.index('endif()',pos)+len('endif()');a=a[:pos]+'''
if(TR2_STM32_R4T_RECOVERY)
 set(TR2_ARTIFACT_NAME "tr2-r4t-monotone-recovery-NON-PRODUCTION")
endif()
'''+a[pos:];a=a.replace(' if(TR2_STM32_R4S_BENCH)\n  target_sources', ''' if(TR2_STM32_R4T_RECOVERY)
  target_sources(tr2_stm32_p11c PRIVATE stm32_r4t_recovery.c r4t_maintenance.c fifo_event_time.c)
  target_compile_definitions(tr2_stm32_p11c PRIVATE TR2_FIFO_TIME_PROFILE=1)
  set_source_files_properties(stm32_r4t_recovery.c r4t_maintenance.c fifo_event_time.c
   "${TR2_FIRMWARE_ROOT}/src/application/live_metrology.c"
   "${TR2_FIRMWARE_ROOT}/src/domain/acquisition/window_statistics.c"
   "${TR2_FIRMWARE_ROOT}/src/domain/supervision/indicator_calculator.c" PROPERTIES COMPILE_OPTIONS "-O2")
 elseif(TR2_STM32_R4S_BENCH)
  target_sources''');f.write_text(a)
# Freeze separate build dirs. No destructive operation or physical call here.
p=Path('/home/lolo/dev/msm/reports/TR2-r4t');f=p/'build-probe.py';a=Path('/home/lolo/dev/msm/reports/TR2-r4s/build-probe-hotleaf.py').read_text().replace('MSM_vibe-R4S-service-budget-safe-recovery','MSM_vibe-R4T-monotonic-recovery').replace('TR2-r4s/probe-build-03','TR2-r4t/probe-build-01').replace('probe-build-06.log','probe-build-01.log').replace("'R4S_BENCH']","'R4S_BENCH','R4T_RECOVERY']");f.write_text(a)
f=p/'hardware/hardware.py';f.write_text(Path('/home/lolo/dev/msm/reports/TR2-r4s/hardware/hardware.py').read_text().replace('TR2-r4s/hardware','TR2-r4t/hardware'))
f=p/'hardware/preflight.py';f.write_text(Path('/home/lolo/dev/msm/reports/TR2-r4s/hardware/preflight.py').read_text().replace('R4R','R4S'))
