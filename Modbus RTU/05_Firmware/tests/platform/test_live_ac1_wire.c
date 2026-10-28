#define TR2_LIVE_MODBUS_MODEL_LIBRARY 1
#include "test_live_modbus_server.c"
#include "tr2/modbus/register_model.h"
static void copy_evidence(void *context,LiveAc1Hardware *out)
{Model *m=context;FifoLive *f=&m->acquisition.fifo;out->run_id=f->run_id;out->full_scale=f->full_scale;out->ctrl1=f->ctrl1_verified;out->valid=f->readback_valid;out->sequence=f->readback_sequence;}
int main(void)
{
 Model *m=live_model();uint8_t q[5]={3,0x1F,0x40,0,64},out[253];
 assert(!modbus_register_model_range_exists(8000,64)); /* no normative collision/allocation */
 assert(pdu(m,q,5,out).response_length==2&&out[1]==2);
 assert(live_modbus_server_enable_ac1(&server,copy_evidence,m)==TR2_OK);
 metric.dynamic.enabled=true;assert(live_modbus_server_publish(&server,m->us/1000)==TR2_OK);
 assert(pdu(m,q,5,out).response_length==130&&out[2]==0x41&&out[3]==0x43&&out[5]==1);
 capture(m,1,1,13,0);drain(m);
 uint64_t until=m->us+3000000;while(m->us<until){assert(tr2_cooperative_enter(&m->work)==TR2_OK);advance_model(m,50);assert(tr2_cooperative_checkpoint(&m->work)==TR2_OK);assert(tr2_cooperative_leave(&m->work)==TR2_OK);}
 assert(metric.dynamic.valid);assert(live_modbus_server_publish(&server,m->us/1000)==TR2_OK);
 assert(pdu(m,q,5,out).response_length==130&&(out[7]&7)==7&&(out[7]&16));
 assert(m->acquisition.fifo.full_scale==2&&out[13]==0xAC); /* actual model uses LIVE_A, +/-8g */
 capture(m,1,2,14,0);drain(m);assert(live_modbus_server_publish(&server,m->us/1000)==TR2_OK);
 assert(pdu(m,q,5,out).response_length==130&&(out[7]&9)==9&&!(out[7]&6));
 q[2]=0x7F;q[4]=2;assert(pdu(m,q,5,out).response_length==2&&out[1]==2);
 uint16_t denied=0;write_words(m,8000,&denied,1,2);
 assert(!modbus_register_model_range_exists(8000,64));
 model_free(m);puts("AC1 opt-in/read-only/magic/version/physical FS/valid/HELD/range guards, normative map untouched: PASS");return 0;
}
