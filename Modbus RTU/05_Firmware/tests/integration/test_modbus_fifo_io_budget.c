#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "tr2/application/production_application.h"
#include "fifo_cooperative.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/persistence/campaign_data_store_persistent_composition.h"
#include "tr2/platform_host/host_platform.h"

typedef struct {
    SerialTransportEvent events[MODBUS_RTU_ADU_MAX_SIZE + 4u];
    size_t count;
    size_t index;
    uint8_t response[MODBUS_RTU_ADU_MAX_SIZE];
    size_t response_length;
    unsigned tx_calls;
    unsigned start_calls;
    Tr2Result tx_result;
    Tr2Result start_result;
    bool poll_failure;
} TestSerial;

static Tr2Result start(void *context)
{
    TestSerial *serial = context;
    serial->start_calls++;
    return serial->start_result;
}

static Tr2Result transmit(void *context, const uint8_t *data, size_t length)
{
    TestSerial *serial = context;
    serial->tx_calls++;
    if (serial->tx_result == TR2_OK) {
        assert(length <= sizeof(serial->response));
        memcpy(serial->response, data, length);
        serial->response_length = length;
    }
    return serial->tx_result;
}

static bool poll(void *context, SerialTransportEvent *event)
{
    TestSerial *serial = context;
    if (serial->poll_failure) return false;
    *event = (SerialTransportEvent){SERIAL_TRANSPORT_EVENT_NONE, 0u,
                                   SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
    if (serial->index < serial->count) *event = serial->events[serial->index++];
    return true;
}

static void load(TestSerial *serial, uint8_t address, const uint8_t *pdu,
                 size_t length, bool corrupt)
{
    uint8_t adu[MODBUS_RTU_ADU_MAX_SIZE];
    size_t adu_length;
    assert(modbus_rtu_adu_encode(address, pdu, length, adu, sizeof(adu),
                                 &adu_length) == MODBUS_RTU_CODEC_OK);
    if (corrupt) adu[adu_length - 1u] ^= 1u;
    serial->count = serial->index = 0u;
    for (size_t i = 0u; i < adu_length; i++) {
        serial->events[serial->count++] = (SerialTransportEvent){
            SERIAL_TRANSPORT_EVENT_BYTE, adu[i], SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
    }
    serial->events[serial->count++] = (SerialTransportEvent){
        SERIAL_TRANSPORT_EVENT_SILENCE_T1_5, 0u, SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
    serial->events[serial->count++] = (SerialTransportEvent){
        SERIAL_TRANSPORT_EVENT_SILENCE_T3_5, 0u, SERIAL_TRANSPORT_ERROR_UNSPECIFIED};
}

static Tr2Result run(ModbusSystemServer *server, TestSerial *serial)
{
    Tr2Result result = TR2_OK;
    while (serial->index < serial->count) {
        result = modbus_system_server_poll_once(server);
        if (result != TR2_OK) return result;
    }
    return result;
}

static ModbusRtuAduView response(TestSerial *serial)
{
    ModbusRtuAduView view;
    assert(modbus_rtu_adu_decode(serial->response, serial->response_length, &view)
           == MODBUS_RTU_CODEC_OK);
    assert(view.unit_id == 17u);
    return view;
}

static void exception(TestSerial *serial, uint8_t function, uint8_t code)
{
    ModbusRtuAduView view = response(serial);
    assert(view.pdu_length == 2u && view.pdu[0] == (uint8_t)(function | 0x80u));
    if (view.pdu[1] != code) {
        fprintf(stderr, "FC %u: exception %u, expected %u\n", function, view.pdu[1], code);
    }
    assert(view.pdu[1] == code);
}

/* An independent elapsed-time model: FRAM 2.5 MHz +20us transaction overhead,
 * FIFO producer 30 words/ms (26k7 XYZ +1/8 timestamps), hardware depth512.
 * Calls reach the actual host medium, bounded journal, engine and PDU/RTU.
 * These are conservative simulated costs, never physical timing claims. */
static uint64_t now_us, produced_remainder;
static uint32_t level, max_level, produced, sequence, xyz, stamp, overrun;
static uint8_t registers[128], words[36864*7];
static FifoCooperative fifo;
static bool pending_start, count_io, media_failure;
static uint32_t media_reads, media_writes, media_commits;
static uint64_t io_us;
static PersistentMedia underlying;
static HostPlatform platform;

static void advance(uint64_t us)
{
    now_us += us;
    if (registers[16] & 0xE0u) {
        produced_remainder += us * 30u;
        uint32_t added = (uint32_t)(produced_remainder / 1000u);
        produced_remainder %= 1000u;
        level += added; produced += added;
        if (level > max_level) max_level = level;
        if (level >= 512u) overrun = 1u;
    }
    platform.monotonic_ms = now_us / 1000u;
}
static void cost(size_t n)
{
    uint64_t us = 20u + ((n + 4u) * 32u + 9u) / 10u;
    io_us += us; advance(us);
}
static Tr2Result media_read(void *context,uint32_t offset,void *buffer,size_t n)
{
    (void)context;
    if (count_io) { ++media_reads; cost(n); }
    if (media_failure) return TR2_ERROR_STORAGE;
    return underlying.read(underlying.context,offset,buffer,n);
}
static Tr2Result media_write(void *context,uint32_t offset,const void *buffer,size_t n)
{
    (void)context;
    if (count_io) { ++media_writes; cost(n); }
    if (media_failure) return TR2_ERROR_STORAGE;
    return underlying.write(underlying.context,offset,buffer,n);
}
static Tr2Result media_commit(void *context)
{
    (void)context;
    if (count_io) { ++media_commits; cost(51914u*2u); }
    if (media_failure) return TR2_ERROR_STORAGE;
    return underlying.commit(underlying.context);
}
static uint32_t tick(void) { return (uint32_t)(now_us/1000u); }
static void delay(uint32_t ms) { advance(ms*1000u); }
static uint32_t stop(void) { return 0; }
static void cleanup(void) { }
static uint8_t tag(uint32_t kind,uint32_t count)
{
    uint8_t value = (uint8_t)((kind<<3)|(count<<1));
    return value | (__builtin_parity(value)&1);
}
static int32_t read_sensor(void *context,uint8_t reg,uint8_t *data,uint16_t n)
{
    (void)context; advance(20u + n*2u); /* SPI4 MHz */
    if (reg == 58u) {
        data[0]=(uint8_t)level; data[1]=(uint8_t)(level>>8);
        if (overrun) data[1] |= 0x60u;
        return 0;
    }
    if (reg == 120u) {
        assert(n%7u==0u && n/7u<=level); level-=n/7u;
        for (unsigned i=0;i<n;i+=7u) {
            memset(data+i,0,7u);
            if (sequence%9u==0u) {
                data[i]=tag(4u,0u); stamp+=24u; memcpy(data+i+1u,&stamp,4u);
            } else {
                data[i]=tag(2u,xyz%4u);uint16_t value=(uint16_t)(xyz%1000u);
                memcpy(data+i+1u,&value,2u);++xyz;
            }
            ++sequence;
        }
        return 0;
    }
    assert(reg+n<=128u);memcpy(data,registers+reg,n);return 0;
}
static int32_t write_sensor(void *context,uint8_t reg,const uint8_t *data,uint16_t n)
{
    (void)context;advance(20u+n*2u);
    if (reg==18u && n==1u && (data[0]&1u)) {
        memset(registers,0,sizeof(registers));registers[15]=0x7Bu;registers[18]=4u;
        level=0u;return 0;
    }
    memcpy(registers+reg,data,n);
    if (reg==10u && !(data[0]&7u)) level=0u;
    return 0;
}
static bool io_available(void *context)
{
    (void)context;return fifo_cooperative_command_io_available(&fifo,pending_start);
}
static void service(ModbusSystemServer *server,TestSerial *serial)
{
    while (serial->index < serial->count) {
        advance(100u); /* independent UART/loop scheduling cost */
        assert(modbus_system_server_poll_once(server)==TR2_OK);
        fifo_cooperative_poll(&fifo);
    }
}
int main(void)
{
    static ProductionApplication application;
    static CampaignDataStorePersistentComposition historical;
    SystemRuntimeDependencies deps={0};
    ConfigurationValidationEnvironment environment={true,4096u};
    TestSerial serial={0};SerialTransport transport={&serial,start,transmit,poll};
    ModbusSystemServer server={0};ModbusSystemServerBinding binding={0};
    ModbusBlock0Image identity={{0u},0u};
    host_platform_init(&platform);underlying=host_platform_persistent_media(&platform);
    PersistentMedia media={NULL,media_read,media_write,media_commit};
    MonotonicClock monotonic=host_platform_monotonic_clock(&platform);
    WallClock wall=host_platform_wall_clock(&platform);
    ResetCauseProvider reset=host_platform_reset_cause_provider(&platform);
    TimeContinuityEvidenceProvider continuity=host_platform_time_continuity_evidence_provider(&platform);
    VibrationSource vibration=host_platform_vibration_source(&platform);
    assert(campaign_data_store_persistent_composition_init(&historical,&media)==TR2_OK);
    deps.monotonic_clock=&monotonic;deps.wall_clock=&wall;deps.reset_cause_provider=&reset;
    deps.time_continuity_evidence_provider=&continuity;deps.persistent_media=&media;
    deps.vibration_source=&vibration;deps.configuration_validation_environment=&environment;
    deps.campaign_data_store=campaign_data_store_persistent_interface(&historical.store);
    assert(production_application_boot(&application,&deps,NULL)==TR2_OK);
    assert(production_application_binding(&application,&transport,17u,&identity,&binding)==TR2_OK);
    binding.refresh_status=system_runtime_build_boot_status_images;
    assert(modbus_system_server_init(&server,&binding)==TR2_OK);
    assert(modbus_system_server_start(&server)==TR2_OK);
    SystemRuntime *runtime=&application.runtime;
    runtime->b3_image_available=true;
    runtime->supervision_service.has_snapshot=true;
    runtime->supervision_service.snapshot.values_available=true;
    runtime->supervision_service.snapshot.calculation_sequence=1u;
    assert(modbus_project_b3(&runtime->supervision_service.snapshot,&runtime->b3_image)==TR2_OK);
    const uint8_t b0[]={3,0,0,0,2},b1[]={3,3,0xE8,0,1},b3[]={3,0x0B,0xB8,0,1};
    const uint8_t b5[]={3,0x13,0x88,0,20};
    uint8_t command[]={16,0x13,0x88,0,8,16,0,99,0,47,0,0,0,0,0,0,0,0,0,0,0,1};
    count_io=true;
    load(&serial,17,b5,sizeof(b5),false);assert(run(&server,&serial)==TR2_OK);
    assert(response(&serial).pdu[0]==3u && media_reads>=512u && io_us>17000u);
    uint64_t baseline_us=io_us;
    stmdev_ctx_t ctx={.read_reg=read_sensor,.write_reg=write_sensor};
    BenchClock clock={tick,delay,stop,cleanup};registers[15]=0x7Bu;registers[18]=4u;
    assert(fifo_cooperative_init(&fifo,&ctx,&clock,words,36864u,32768u)==0);
    assert(!io_available(NULL));
    while (fifo.state==COOP_CONFIGURING) { advance(1000u);fifo_cooperative_poll(&fifo); }
    assert(fifo.state==COOP_ARMED && io_available(NULL));
    server.binding.command_io_available=io_available;pending_start=true;
    uint32_t reads_before=media_reads,writes_before=media_writes,commits_before=media_commits;
    load(&serial,17,command,sizeof(command),false);service(&server,&serial);
    exception(&serial,16u,MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);
    assert(runtime->command_mailbox.transaction_id==0u && writes_before==media_writes);
    assert(fifo_cooperative_start(&fifo)==0);pending_start=false;
    while (fifo.state==COOP_CONFIGURING) { advance(100u);fifo_cooperative_poll(&fifo); }
    assert(fifo.state==COOP_CAPTURING && !io_available(NULL));
    for (unsigned iteration=0;iteration<8u;iteration++) {
        const uint8_t *requests[]={b0,b1,b3,b5,command};
        const size_t lengths[]={sizeof(b0),sizeof(b1),sizeof(b3),sizeof(b5),sizeof(command)};
        for (unsigned j=0;j<5u;j++) {
            load(&serial,17,requests[j],lengths[j],false);service(&server,&serial);
            if(j>=3u) exception(&serial,j==3u?3u:16u,MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);
            else assert(response(&serial).pdu[0]==3u);
            assert(media_reads==reads_before && media_writes==writes_before && media_commits==commits_before);
            assert(!overrun && runtime->command_mailbox.transaction_id==0u);
        }
    }
    while(fifo.state!=COOP_READY && fifo.state!=COOP_INVALID) {
        advance(100u);fifo_cooperative_poll(&fifo);
        if(fifo.state==COOP_CLEANUP) assert(!io_available(NULL));
    }
    assert(fifo.state==COOP_READY && io_available(NULL) && !overrun && max_level<512u);
    assert(fifo.window.xyz==32768u && fifo.window.timestamps==4096u);
    /* Same identity may now be admitted durably. Retry must not mutate media. */
    load(&serial,17,command,sizeof(command),false);assert(run(&server,&serial)==TR2_OK);
    assert(response(&serial).pdu[0]==16u && media_writes>writes_before && media_commits>commits_before);
    uint32_t committed=media_commits,written=media_writes;
    load(&serial,17,command,sizeof(command),false);assert(run(&server,&serial)==TR2_OK);
    assert(response(&serial).pdu[0]==16u && media_commits==committed && media_writes==written);
    load(&serial,17,b5,sizeof(b5),false);assert(run(&server,&serial)==TR2_OK);
    assert(response(&serial).pdu[0]==3u && server.b5.source_generation>0u);
    assert(server.b5.registers[15]==47u); /* authoritative durable last */
    command[13]=1u; /* collision, same transaction, different identity */
    load(&serial,17,command,sizeof(command),false);assert(run(&server,&serial)==TR2_OK);
    exception(&serial,16u,MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);
    assert(media_writes==written && media_commits==committed);
    media_failure=true;load(&serial,17,b5,sizeof(b5),false);assert(run(&server,&serial)==TR2_OK);
    exception(&serial,3u,MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);
    assert(server.rtu.pdu_context.read_sources.b5_image==NULL && server.command_snapshot_failures==1u);
    media_failure=false;
    /* Store stays invalid until real recovery, not just restoration of the I/O flag. */
    load(&serial,17,b5,sizeof(b5),false);assert(run(&server,&serial)==TR2_OK);
    exception(&serial,3u,MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE);
    CommandJournalBoundedRecoveryResult recovery;
    assert(command_journal_bounded_store_recover(&runtime->command_journal_store,&recovery)==TR2_OK);
    load(&serial,17,b5,sizeof(b5),false);assert(run(&server,&serial)==TR2_OK);
    assert(response(&serial).pdu[0]==3u && server.b5.registers[15]==47u);
    fifo.cleanup_result=BENCH_IO;assert(!io_available(NULL));
    uint32_t qualified_max_level=max_level;
    /* Negative control: the independent producer must detect the long scan
     * when this test fixture deliberately omits the production admission guard. */
    fifo.cleanup_result=BENCH_OK;max_level=level=overrun=produced_remainder=0u;
    assert(fifo_cooperative_init(&fifo,&ctx,&clock,words,36864u,4096u)==0);
    while(fifo.state==COOP_CONFIGURING){advance(1000u);fifo_cooperative_poll(&fifo);}
    assert(fifo.state==COOP_ARMED && fifo_cooperative_start(&fifo)==0);
    while(fifo.state==COOP_CONFIGURING){advance(100u);fifo_cooperative_poll(&fifo);}
    server.binding.command_io_available=NULL;
    load(&serial,17,b5,sizeof(b5),false);service(&server,&serial);
    assert(overrun && media_reads>reads_before);
    while(fifo.state!=COOP_INVALID && fifo.state!=COOP_READY){advance(100u);fifo_cooperative_poll(&fifo);}
    assert(fifo.state==COOP_INVALID && fifo.result!=BENCH_OK);
    printf("timed real journal: baseline B5=%llu us; protected capture XYZ=32768 TS=4096 maxFIFO=%u OVR=0; denied=%u; durable retry/collision/fault/recovery passed; unguarded negative control OVR=%u INVALID\n",
           (unsigned long long)baseline_us,qualified_max_level,server.command_io_denied_requests,overrun);
    return 0;
}
