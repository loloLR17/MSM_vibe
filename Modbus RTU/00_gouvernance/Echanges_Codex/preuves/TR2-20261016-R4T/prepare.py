from pathlib import Path
import re,zlib
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4T-monotonic-recovery');fw=r/'Modbus RTU/05_Firmware';old=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4S-service-budget-safe-recovery/Modbus RTU/05_Firmware')
# Reviewed semantic capture only, not the failed empty-journal bridge.
s=(old/'platform/stm32/r4s_maintenance.c').read_text();s=s[:s.index('Tr2Result r4s_capture_bridge')].replace('"r4s_maintenance.h"','"r4t_maintenance.h"\n#include "tr2/common/sha256.h"').replace('Tr2Result r4s_capture(','static Tr2Result semantic_capture(')
s+='''
Tr2Result r4t_capture(R4tRetirement *t,const LiveCommandEngine *e,const uint8_t snapshot[262144])
{
 if(!t||!snapshot||!e||!e->store||!e->store->configuration)return TR2_ERROR_INVALID_ARGUMENT;
 memset(t,0,sizeof(*t));uint8_t hash[32];
 static const uint8_t expected[32]={0x14,0xfb,0x17,0x2f,0x29,0x17,0xd0,0xa8,0xe4,0x40,0x43,0x80,0x51,0x83,0x1c,0x84,0xf3,0xf3,0xe7,0x46,0x16,0x70,0x2a,0x7d,0xe9,0xe9,0xa1,0xa2,0x51,0xb2,0xaa,0x9};
 if(tr2_sha256_bytes(snapshot,262144,hash)!=TR2_OK||memcmp(hash,expected,32))return TR2_ERROR_INVALID_STATE;
 const TransactionalImageMedia *m=e->store->configuration->live;
 if(m->generation!=39||!m->recovered||m->active_image>1||
  memcmp(m->candidate,snapshot+(m->active_image?188416u:131072u)+64,52298))return TR2_ERROR_INVALID_STATE;
 Tr2Result result=semantic_capture(&t->gate,e);if(result!=TR2_OK)return result;
 t->generation_floor=e->store->generation;t->archived=e->active;return TR2_OK;
}
Tr2Result r4t_retire_and_admit(R4tRetirement *t,LiveCommandEngine *e)
{
 if(!t||!e||!e->store||!e->store->configuration||t->gate.phase!=1||t->generation_floor!=26)
  return TR2_ERROR_INVALID_STATE;
 R4mMaintenance current;if(semantic_capture(&current,e)!=TR2_OK||
  memcmp(current.intent,t->gate.intent,128)||e->store->generation!=t->generation_floor)return TR2_ERROR_INVALID_STATE;
 LiveCommandStore *s=e->store;TransactionalImageMedia *m=s->configuration->live;
 PersistentMedia *media=transactional_image_media_interface(m);const uint8_t zero[32]={0};
 t->gate.phase=0; /* consumed before staging, including failure: never retry */
 Tr2Result result=tr2_cooperative_enter(m->work);if(result!=TR2_OK)return result;
 /* Retire only slot3 in the candidate, WITHOUT publishing an empty journal.
  * The real R2 admission supplies generation27 in the SAME outer transaction.
  * Unchanged terminal receipts and authentic INTENT remain byte-identical.
  * This isolated maintenance frontend has no normal ingress, even transiently. */
 for(uint32_t at=0;at<224&&result==TR2_OK;at+=32)
  result=media->write(media->context,15922u+224u*3u+at,zero,32);
 if(result==TR2_OK) {
  s->present[3]=false; /* staged namespace retirement, no durable state yet */
  e->has_active=false;e->indeterminate=false;memset(&e->active,0,sizeof(e->active));
  LiveCommandRequest q={0};q.epoch=4;q.command.transaction_id=65535;q.command.identity.command_code=12;
  result=live_command_engine_submit(e,&q);
  if(result==TR2_OK&&(!e->has_active||e->active.phase!=1||e->active.generation!=27||s->generation!=27||
   e->status!=2||s->effect.run_id!=5||memcmp(t->gate.intent,m->candidate+TR2_LIVE_EFFECT_OFFSET,128)))result=TR2_ERROR_INVALID_STATE;
 }
 Tr2Result end=tr2_cooperative_leave(m->work);if(result==TR2_OK)result=end;
 if(result!=TR2_OK){e->booted=false;s->mounted=false;(void)live_command_engine_fault(e);return result;}
 t->gate.phase=2;return TR2_OK;
}
'''
# Correct literal produced from the authenticated SHA, not manual nibble grouping.
sha=bytes.fromhex('14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9')
s=re.sub(r'static const uint8_t expected\[32\]=\{[^}]+\}', 'static const uint8_t expected[32]={'+','.join(hex(x) for x in sha)+'}',s)
(fw/'platform/stm32/r4t_maintenance.c').write_text(s)
(fw/'platform/stm32/r4t_maintenance.h').write_text('''#ifndef R4T_MAINTENANCE_H
#define R4T_MAINTENANCE_H
#include "r4m_maintenance.h"
/* Exact authenticated snapshot only; NOT generic product recovery.
 * Every boot CLOSED. No implicit continuation after any interrupted barrier. */
typedef struct {R4mMaintenance gate;uint64_t generation_floor;LiveCommandRecord archived;} R4tRetirement;
Tr2Result r4t_capture(R4tRetirement *,const LiveCommandEngine *,const uint8_t[262144]);
/* Atomically retire slot3 and admit a REAL RENEW using the unchanged R2 engine.
 * On failure the token is consumed and the store/frontend are unavailable.
 * No persistent empty bridge, counter seed record, invented receipt or epoch. */
Tr2Result r4t_retire_and_admit(R4tRetirement *,LiveCommandEngine *);
#endif
''')
(fw/'tests/fixtures/r4r_started_snapshot.zlib').write_bytes(zlib.compress(Path('/home/lolo/dev/msm/reports/TR2-r4s/hardware/fram-before.bin').read_bytes(),9))
runner=(fw/'tests/platform/run_r4m_maintenance.py').read_text().replace('c814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561','14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9').replace('tr2-r4m-','tr2-r4t-')
(fw/'tests/platform/run_r4t_maintenance.py').write_text(runner)
f=fw/'CMakeLists.txt';t=f.read_text();t+='''
add_executable(test_r4t_maintenance tests/platform/test_r4t_maintenance.c platform/stm32/r4t_maintenance.c platform/stm32/r4m_maintenance.c)
target_include_directories(test_r4t_maintenance PRIVATE platform/stm32)
target_link_libraries(test_r4t_maintenance PRIVATE tr2_core)
add_test(NAME r4t_maintenance COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/platform/run_r4t_maintenance.py
    $<TARGET_FILE:test_r4t_maintenance> ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/r4r_started_snapshot.zlib)
''';f.write_text(t)
