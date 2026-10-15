/* LINK TEST ONLY. No startup, board, HAL or production memory map. DO NOT FLASH. */
#include "tr2/application/live_command_engine.h"
static LiveCommandEngine engine;
static LiveCommandStore store;
static LiveConfiguration config;
static LiveAcquisitionPort port;
static LiveCommandRequest request;
static uint16_t registers[29];
volatile unsigned smoke_run;
int main(void) {
    int result=0;
    while(smoke_run) {
        result+=(int)live_command_store_init(&store,&config);
        result+=(int)live_command_engine_init(&engine,&store,&port);
        result+=(int)live_command_engine_boot(&engine);
        result+=(int)live_command_engine_submit(&engine,&request);
        result+=(int)live_command_engine_step(&engine);
        result+=(int)live_command_engine_project_b5(&engine,registers);
        result+=(int)live_command_engine_read_b5(&engine,5000,registers,29);
        result+=(int)live_command_engine_write_b5(&engine,5020,registers,2);
    }
    return result;
}
