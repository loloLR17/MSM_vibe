/* Normative oracle: literal wire addresses 3002/3003, NOT inferred from the
 * projectors. Reuse input generators only, never the old test expectations. */
#define main retained_live_metrology_regression
#include "test_live_metrology.c"
#undef main
#include "tr2/modbus/codec.h"
static void check(LiveMetrology *s,const char *name,unsigned alarm,unsigned severity)
{
 ModbusBlock3Image b;Tr2Result r=live_metrology_project_b3(s,s->last_ms,&b);
 assert(r==(s->error?TR2_ERROR_INVALID_STATE:TR2_OK));
 assert(b.registers[2]==alarm && b.registers[3]==severity && b.registers[3]<=4);
 uint8_t pdu[98]={3,96},adu[101];size_t size=0;
 for(unsigned i=0;i<48;i++){pdu[2+2*i]=(uint8_t)(b.registers[i]>>8);pdu[3+2*i]=(uint8_t)b.registers[i];}
 assert(modbus_rtu_adu_encode(1,pdu,sizeof(pdu),adu,sizeof(adu),&size)==MODBUS_RTU_CODEC_OK&&size==101);
 assert(adu[7]==(alarm>>8)&&adu[8]==(alarm&255)); /* 3002 */
 assert(adu[9]==0&&adu[10]==severity);          /* 3003 */
 ModbusRtuAduView view;assert(modbus_rtu_adu_decode(adu,size,&view)==MODBUS_RTU_CODEC_OK);
 printf("R5AFIX_FIXTURE,{\"kind\":\"adu\",\"case\":\"%s\",\"expected_alarm\":%u,\"expected_severity\":%u,\"request\":\"01 03 0B B8 00 30 C7 DF\",\"response\":\"",name,alarm,severity);
 for(unsigned i=0;i<size;i++){printf("%s%02X",i?" ":"",adu[i]);}
 puts("\"}");
}
int main(void)
{
 LiveMetrology s;ActiveConfigurationSnapshot c=config();
 c.payload.rms_alarm_threshold_mg=c.payload.peak_alarm_threshold_mg=300;
 live_metrology_init(&s);check(&s,"boot_invalid",0,0);
 assert(live_metrology_start(&s,1,&c)==TR2_OK);check(&s,"waiting",0,0);
 window(&s,100,0);check(&s,"normal_utc_fresh",0,1);
 live_metrology_stop(&s);assert(live_metrology_start(&s,2,&c)==TR2_OK);
 window_utc(&s,100,2000,false,0);check(&s,"fresh_utc_absent",0,2);
 live_metrology_stop(&s);assert(live_metrology_start(&s,3,&c)==TR2_OK);
 window_utc(&s,2000,4000,false,0);check(&s,"warning_exceed",0xF0,3);
 window_utc(&s,6000,6000,false,0);check(&s,"active_alarm_latch",0x1FF,4);
 live_metrology_stop(&s);check(&s,"held_stop",0x1FF,0);
 assert(live_metrology_start(&s,4,&c)==TR2_OK);
 window_utc(&s,0,8000,false,0);check(&s,"latch_without_active_alarm",0x100,2);
 live_metrology_invalidate(&s,true,false);check(&s,"calculation_error_held",0x100,0);
 puts("Normative 3002 alarms / 3003 severity: nine literal word and ADU oracles PASS");return 0;
}
