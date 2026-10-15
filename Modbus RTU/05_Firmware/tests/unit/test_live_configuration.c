#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tr2/application/live_configuration.h"
#include "tr2/common/sha256.h"
#include "tr2/modbus/b4_configuration_codec.h"
#include "tr2/persistence/configuration_record.h"
#include "live_r0_vectors.h"

typedef struct {
    uint8_t bytes[262144];
    size_t budget, written, reads, read_bytes, writes, fail_read_after_write;
    bool owner, protect, fail_read;
} Physical;
typedef struct {
    Physical physical;
    uint8_t candidate[52298];
    TransactionalImageMedia historical, live;
    LiveConfiguration service;
    PersistentIdentityAssignment assignment;
} Fixture;
static const uint32_t uid[3]={0x12345678u,0xABCDEF01u,0xFEDCBA98u};
static Tr2Result rd(void *context,uint32_t offset,void *buffer,size_t size)
{
    Physical *p=context;p->reads++;p->read_bytes+=size;
    assert(offset<=sizeof(p->bytes)&&size<=sizeof(p->bytes)-offset);
    if(p->fail_read) return TR2_ERROR_STORAGE;
    memcpy(buffer,p->bytes+offset,size);return TR2_OK;
}
static Tr2Result wr(void *context,uint32_t offset,const void *buffer,size_t size)
{
    Physical *p=context;size_t count=size<p->budget?size:p->budget;
    assert(offset<=sizeof(p->bytes)&&size<=sizeof(p->bytes)-offset);
    if(p->protect) assert(offset>=122880u&&offset+size<=245760u);
    p->writes++;memcpy(p->bytes+offset,buffer,count);p->written+=count;
    p->budget-=count;
    if(p->fail_read_after_write==p->writes) p->fail_read=true;
    return count==size?TR2_OK:TR2_ERROR_STORAGE;
}
static Tr2Result owner(void *context,uint32_t offset,size_t size)
{
    Physical *p=context;assert(offset>=122880u&&offset+size<=245760u);
    return p->owner?TR2_OK:TR2_ERROR_NOT_AVAILABLE;
}
static void boot(Fixture *f)
{
    TransactionalImagePhysicalStorage storage={&f->physical,rd,wr};
    TransactionalImageGeometry geometry=transactional_image_geometry_qualification_profile();
    memcpy(f->assignment.hardware_uid,uid,sizeof(uid));f->assignment.device_id=1u;
    assert(transactional_image_media_init_v2(&f->historical,&storage,&geometry,
        f->candidate,sizeof(f->candidate))==TR2_OK);
    assert(transactional_image_media_init_live(&f->live,&storage,f->candidate,
        sizeof(f->candidate),&f->physical,owner)==TR2_OK);
    assert(live_configuration_init(&f->service,&f->historical,&f->live,uid,
        &f->assignment,1u)==TR2_OK);
}
/* Independent register-to-fixture mapping, including nontrivial LIVE_B labels. */
static PreparedConfiguration prepared(const uint16_t *r,uint32_t id)
{
    PreparedConfiguration q={0};ConfigurationPayload *p=&q.payload;unsigned i;
    q.config_id=id;p->sampling_frequency_hz=r[0];p->axes_enable_mask=r[2];
    p->full_scale_code=r[3];p->acquisition_mode=r[4];p->window_size_samples=r[5];
    p->indicator_period_ms=r[6];p->campaign_duration_s=((uint32_t)r[7]<<16)|r[8];
    p->storage_mode=r[9];p->storage_limit_mb=((uint32_t)r[10]<<16)|r[11];
    p->supervision_enable_mask=r[24];p->rms_warn_threshold_mg=r[25];
    p->rms_alarm_threshold_mg=r[26];p->peak_warn_threshold_mg=r[27];
    p->peak_alarm_threshold_mg=r[28];p->threshold_hysteresis_mg=r[29];
    p->alarm_hold_time_ms=r[30];p->campaign_context_id=((uint32_t)r[40]<<16)|r[41];
    p->mission_id=((uint32_t)r[42]<<16)|r[43];
    for(i=0u;i<32u;++i) {
        p->campaign_label[i]=(char)(r[44u+i/2u]>>(i%2u?0u:8u));
        p->mission_label[i]=(char)(r[60u+i/2u]>>(i%2u?0u:8u));
    }
    p->operating_mode_code=r[76];p->navigation_zone_code=r[77];
    p->load_state_code=r[78];p->sea_state_code=r[79];
    q.supplied_crc=tr2_b4_prepared_payload_crc(p);return q;
}
static Fixture *fixture(void)
{
    Fixture *f=calloc(1,sizeof(*f));uint8_t *image=calloc(1,51914u);
    ActiveConfigurationSnapshot config={0};PersistentIdentityRecord identity={0};
    PreparedConfiguration old=prepared(vector_0_prepared,1u);
    assert(f&&image);memset(f->physical.bytes,0xFF,sizeof(f->physical.bytes));
    f->physical.budget=SIZE_MAX;f->physical.owner=true;boot(f);
    memset(image,0xAC,140u);config.generation=1u;config.config_id=1u;
    config.revision_counter=1u;config.payload=old.payload;
    assert(tr2_configuration_record_encode(&config,image+140u,140u)==TR2_OK);
    memset(image+15922u,0x93,35840u);memset(image+51802u,0x75,16u);
    identity.generation=1u;memcpy(identity.identity.hardware_uid,uid,sizeof(uid));
    identity.identity.functional.device_id=1u;identity.identity.functional.generation=1u;
    identity.identity.functional.protocol_version=1u;
    identity.identity.functional.device_capabilities=15u;identity.identity.unit_id=1u;
    memcpy(identity.identity.functional.serial_number,"HOST-R1",7u);
    memcpy(identity.identity.functional.manufacturer,"MSM",3u);
    assert(persistent_identity_encode(&identity,image+51818u)==TR2_OK);
    assert(transactional_image_media_publish_v2(&f->historical,image,51914u,0u)==TR2_OK);
    free(image);f->physical.protect=true;f->physical.writes=0u;f->physical.written=0u;
    return f;
}
static void check_vector(const uint16_t *pr,const uint16_t *ar,uint32_t pc,uint32_t ac)
{
    PreparedConfiguration p=prepared(pr,2u);uint16_t regs[84];
    tr2_b4_serialize_prepared_payload(&p.payload,regs);
    assert(memcmp(regs,pr,168u)==0&&p.supplied_crc==pc);
    tr2_b4_serialize_active_payload(&p.payload,regs);
    assert(memcmp(regs,ar,152u)==0&&tr2_b4_active_payload_crc(&p.payload)==ac);
}
static void validator_and_codecs(void)
{
    PreparedConfiguration q=prepared(vector_1_prepared,2u),bad;
    ActiveConfigurationSnapshot a={0},out;uint8_t record[140];
    const uint16_t windows[]={4096,8192,16384,32768};
    const uint16_t periods[]={2000,5000,10000,30000,60000};
    unsigned w,p,x,s;ConfigurationValidationEnvironment environment={0};
    check_vector(vector_0_prepared,vector_0_active,crc_0_prepared,crc_0_active);
    check_vector(vector_1_prepared,vector_1_active,crc_1_prepared,crc_1_active);
    check_vector(vector_2_prepared,vector_2_active,crc_2_prepared,crc_2_active);
    assert(configuration_validate_live(NULL,NULL).status!=CONFIGURATION_VALIDATION_VALID);
    assert(configuration_validate(&q,&environment,NULL).status!=CONFIGURATION_VALIDATION_VALID);
    for(w=0;w<4;w++)for(p=0;p<5;p++)for(x=1;x<=7;x++)for(s=0;s<4;s++) {
        q.payload.window_size_samples=windows[w];q.payload.indicator_period_ms=periods[p];
        q.payload.axes_enable_mask=(uint16_t)x;q.payload.full_scale_code=(uint16_t)s;
        assert(configuration_validate_live(&q,NULL).status==CONFIGURATION_VALIDATION_VALID);
    }
#define REJECT(field,value) do {bad=q;bad.field=value;assert(configuration_validate_live(&bad,NULL).status!=CONFIGURATION_VALIDATION_VALID);} while(0)
    REJECT(config_id,0u);REJECT(payload.acquisition_mode,1u);REJECT(payload.storage_mode,1u);
    REJECT(payload.storage_limit_mb,1u);REJECT(payload.campaign_duration_s,60u);
    REJECT(payload.axes_enable_mask,0u);REJECT(payload.axes_enable_mask,8u);
    REJECT(payload.full_scale_code,4u);REJECT(payload.sampling_frequency_hz,26666u);
    REJECT(payload.window_size_samples,4095u);REJECT(payload.indicator_period_ms,1999u);
    REJECT(payload.supervision_enable_mask,0u);REJECT(payload.supervision_enable_mask,2u);
    REJECT(payload.rms_warn_threshold_mg,65535u);REJECT(payload.peak_warn_threshold_mg,65535u);
    REJECT(payload.campaign_context_id,0u);REJECT(payload.mission_id,0u);
    REJECT(payload.campaign_label[31],(char)0x80);REJECT(payload.mission_label[0],31);
#undef REJECT
    q.payload.threshold_hysteresis_mg=65535u;q.payload.alarm_hold_time_ms=65535u;
    q.payload.rms_warn_threshold_mg=0u;q.payload.rms_alarm_threshold_mg=0u;
    assert(configuration_validate_live(&q,NULL).status==CONFIGURATION_VALIDATION_VALID);
    a.generation=1u;a.config_id=2u;a.revision_counter=1u;a.payload=q.payload;
    assert(tr2_configuration_record_encode_live(&a,record,140u)==TR2_OK);
    assert(tr2_configuration_record_decode(record,140u,&out)==TR2_ERROR_UNSUPPORTED);
    assert(tr2_configuration_record_decode_live(record,140u,&out)==TR2_OK);
    assert(tr2_configuration_record_encode(&a,record,140u)==TR2_OK);
    assert(tr2_configuration_record_decode_live(record,140u,&out)==TR2_ERROR_UNSUPPORTED);
    puts("R0 CRC vectors, 560 LIVE domain combinations, refusal and version isolation: PASS");
}
static void sha_vectors(void)
{
    static const char *expected[]={
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
        "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
        "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"};
    const char *input[]={"","abc","abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"};
    uint8_t digest[32];char text[65];unsigned i,j;uint8_t *million=malloc(1000000u);assert(million);
    memset(million,'a',1000000u);
    for(i=0;i<4;i++) {
        assert(tr2_sha256_bytes(i<3?(const uint8_t*)input[i]:million,
            i<3?strlen(input[i]):1000000u,digest)==TR2_OK);
        for(j=0;j<32;j++) (void)sprintf(text+2*j,"%02x",digest[j]);
        assert(strcmp(text,expected[i])==0);
    }
    assert(tr2_sha256_bytes(NULL,1u,digest)==TR2_ERROR_INVALID_ARGUMENT);free(million);
    puts("SHA256 empty/abc/two-block/million-a: PASS");
}
static void nominal_and_guards(void)
{
    Fixture *f=fixture();uint8_t *before=malloc(262144u);PreparedConfiguration q=prepared(vector_1_prepared,2u),bad;
    ActiveConfigurationSnapshot active;PersistentIdentityRecord identity,decoded;uint8_t record[96];
    ModbusBlock4Image b4;size_t reads,writes;
    assert(before);memcpy(before,f->physical.bytes,262144u);
    f->physical.owner=false;
    assert(live_configuration_migrate(&f->service)!=TR2_OK&&f->physical.writes==0u);
    f->physical.owner=true;assert(live_configuration_migrate(&f->service)==TR2_OK);
    assert(!live_configuration_active(&f->service,&active));
    assert(memcmp(f->candidate+280u,before+8192u+64u+280u,15642u)==0);
    assert(memcmp(f->candidate+51762u,before+8192u+64u+51762u,40u)==0);
    assert(live_configuration_identity(&f->service,&identity)==TR2_OK);
    assert(identity.identity.functional.protocol_version==2u&&identity.identity.functional.device_capabilities==12u);
    assert(persistent_identity_encode_live(&identity,record)==TR2_OK);
    assert(persistent_identity_decode(record,96u,&decoded)==TR2_ERROR_UNSUPPORTED);
    assert(persistent_identity_decode_live(record,96u,&decoded)==TR2_OK);
    assert(live_configuration_migrate(&f->service)==TR2_ERROR_INVALID_STATE);
    reads=f->physical.reads;writes=f->physical.writes;
    assert(live_configuration_apply(&f->service,&q,true)==TR2_ERROR_INVALID_STATE);
    bad=q;bad.supplied_crc^=1u;assert(live_configuration_apply(&f->service,&bad,false)==TR2_ERROR_CORRUPTED);
    bad=q;bad.payload.storage_mode=1u;bad.supplied_crc=tr2_b4_prepared_payload_crc(&bad.payload);
    assert(live_configuration_apply(&f->service,&bad,false)==TR2_ERROR_INVALID_ARGUMENT);
    assert(reads==f->physical.reads&&writes==f->physical.writes);
    f->physical.written=0;reads=f->physical.read_bytes;assert(live_configuration_apply(&f->service,&q,false)==TR2_OK);
    assert(f->physical.written==52426u&&live_configuration_active(&f->service,&active));
    assert(active.config_id==2u&&active.generation==2u&&active.revision_counter==2u);
    assert(live_configuration_project_b4(&f->service,&q,&b4)==TR2_OK);
    assert(b4.registers[0]==2u&&b4.registers[1]==1u);
    assert(memcmp(b4.registers+100u,vector_1_active,152u)==0);
    assert(memcmp(b4.registers+16u,vector_1_prepared,168u)==0);
    assert((((uint32_t)b4.registers[8]<<16)|b4.registers[9])==crc_1_prepared);
    assert((((uint32_t)b4.registers[10]<<16)|b4.registers[11])==crc_1_active);
    printf("Apply mock I/O: %zu read bytes, %zu write bytes\n",f->physical.read_bytes-reads,f->physical.written);
    assert(memcmp(before,f->physical.bytes,122880u)==0);
    assert(memcmp(before+245760u,f->physical.bytes+245760u,16384u)==0);
    boot(f);assert(live_configuration_recover(&f->service)==TR2_OK);
    assert(live_configuration_active(&f->service,&active)&&active.config_id==2u);
    f->physical.fail_read=true;assert(live_configuration_recover(&f->service)!=TR2_OK);
    assert(!live_configuration_active(&f->service,&active));
    printf("Real-store migration/apply/reboot, protected bytes, no SD: PASS; commit=52426 bytes; host service=%zu media=%zu candidate=%zu\n",
        sizeof(LiveConfiguration),sizeof(TransactionalImageMedia),sizeof(f->candidate));
    free(before);free(f);
}
static void power_cuts(bool migration)
{
    Fixture *f=fixture();uint8_t *baseline=malloc(262144u);PreparedConfiguration a=prepared(vector_1_prepared,2u),b=prepared(vector_2_prepared,3u);
    size_t cut,cases=0u;ActiveConfigurationSnapshot active;
    assert(baseline);
    if(!migration) {assert(live_configuration_migrate(&f->service)==TR2_OK);assert(live_configuration_apply(&f->service,&a,false)==TR2_OK);}
    memcpy(baseline,f->physical.bytes,262144u);
    /* Payload every 256 bytes and every header/root byte boundary, not every payload byte. */
    for(cut=0u;cut<=52426u;cut+=(cut<52224u?256u:1u)) {
        Tr2Result result;memcpy(f->physical.bytes,baseline,262144u);f->physical.budget=SIZE_MAX;boot(f);
        f->physical.budget=cut;
        result=migration?live_configuration_migrate(&f->service):live_configuration_apply(&f->service,&b,false);
        if(cut==0u) assert(result!=TR2_OK);
        f->physical.budget=SIZE_MAX;boot(f);result=live_configuration_recover(&f->service);
        if(!migration) assert(result==TR2_OK&&live_configuration_active(&f->service,&active));
        if(live_configuration_active(&f->service,&active)) {
            assert(!migration&&result==TR2_OK);
            assert(active.config_id==2u||active.config_id==3u);
            assert(tr2_b4_active_payload_crc(&active.payload)==
                (active.config_id==2u?crc_1_active:crc_2_active));
        }
        assert(memcmp(baseline,f->physical.bytes,122880u)==0);
        assert(memcmp(baseline+245760u,f->physical.bytes+245760u,16384u)==0);cases++;
    }
    printf("%s power-cut byte budgets: %zu cases, historical prefix/tail preserved: PASS\n",migration?"Migration":"Activation",cases);
    free(baseline);free(f);
}
static uint32_t crc_bytes(const uint8_t *bytes,size_t size)
{
    uint32_t c=UINT32_MAX;size_t i;unsigned bit;
    for(i=0;i<size;i++) {c^=bytes[i];for(bit=0;bit<8;bit++) c=(c&1u)?(c>>1)^0xEDB88320u:c>>1;}
    return c^UINT32_MAX;
}
static void root_crc(uint8_t *root)
{
    uint32_t c=crc_bytes(root,60u);unsigned i;
    for(i=0;i<4;i++) root[60u+i]=(uint8_t)(c>>(24u-8u*i));
}
static void raw_change(Fixture *f,size_t offset,const uint8_t *bytes,size_t size)
{
    assert(f->live.interface.write(&f->live,(uint32_t)offset,bytes,size)==TR2_OK);
    assert(f->live.interface.commit(&f->live)==TR2_OK);
}
static void corruption_and_limits(void)
{
    Fixture *f=fixture();uint8_t *baseline=malloc(262144u),record[140];
    PreparedConfiguration q=prepared(vector_1_prepared,2u);ActiveConfigurationSnapshot active;
    PersistentIdentityRecord id;TransactionalImageRecoveryResult recovery;unsigned which;
    assert(baseline);assert(live_configuration_migrate(&f->service)==TR2_OK);
    assert(live_configuration_apply(&f->service,&q,false)==TR2_OK);
    memcpy(baseline,f->physical.bytes,262144u);
    for(which=0;which<11;which++) {
        uint8_t byte=1u;size_t writes;
        memcpy(f->physical.bytes,baseline,262144u);boot(f);
        assert(live_configuration_recover(&f->service)==TR2_OK);
        if(which==0u) { /* highest published image corrupted: never fall back */
            size_t bank=f->live.active_image?188416u:131072u;
            f->physical.bytes[bank+64u+1000u]^=1u;
        } else if(which==1u) { /* highest CRC-valid publication is future schema */
            uint8_t *root=f->physical.bytes+(f->live.active_superblock?126976u:122880u);
            root[7]=4u;root_crc(root);
        } else if(which==2u) { /* equal-generation conflicting publication */
            uint8_t *a=f->physical.bytes+122880u,*b=f->physical.bytes+126976u;
            memcpy(a+8,b+8,8u);a[17]=1u;root_crc(a);
        } else if(which==3u) { /* record damaged inside otherwise valid image */
            byte=f->candidate[80u]^1u;raw_change(f,80u,&byte,1u);
        } else if(which==4u) { /* R2 effect reservation is not replayed by R1 */
            raw_change(f,TR2_LIVE_EFFECT_OFFSET,&byte,1u);
        } else if(which==5u) { /* historical command journal cannot be reintroduced */
            raw_change(f,15922u,&byte,1u);
        } else if(which==6u) { /* valid identity CRC, wrong hardware UID */
            assert(live_configuration_identity(&f->service,&id)==TR2_OK);
            id.identity.hardware_uid[0]^=1u;
            assert(persistent_identity_encode_live(&id,record)==TR2_OK);
            raw_change(f,51818u,record,96u);
        } else if(which==7u) { /* provenance must still identify the old image */
            f->physical.protect=false;
            assert(transactional_image_media_recover(&f->historical,&recovery)==TR2_OK);
            assert(f->historical.interface.write(&f->historical,1000u,&byte,1u)==TR2_OK);
            assert(f->historical.interface.commit(&f->historical)==TR2_OK);
            f->physical.protect=true;
        } else if(which==8u) { /* epoch R2 not supported by anchor-only R1 */
            byte=2u;raw_change(f,TR2_LIVE_EPOCH_OFFSET+19u,&byte,1u);
        } else if(which==10u) {
            assert(live_configuration_identity(&f->service,&id)==TR2_OK);
            id.identity.functional.device_capabilities|=16u;
            assert(persistent_identity_encode_live(&id,record)==TR2_OK);
            raw_change(f,51818u,record,96u);
        } else { /* readable medium failure is explicit unavailability */
            f->physical.fail_read=true;
        }
        writes=f->physical.writes;boot(f);
        if(live_configuration_recover(&f->service)==TR2_OK) {fprintf(stderr,"unexpected recovery case %u\n",which);assert(false);}
        assert(!live_configuration_active(&f->service,&active));
        assert(f->physical.writes==writes);f->physical.fail_read=false;
        if(which!=7u) assert(memcmp(baseline,f->physical.bytes,122880u)==0);
    }
    for(which=0;which<2;which++) {
        size_t writes;memcpy(f->physical.bytes,baseline,262144u);boot(f);
        assert(live_configuration_recover(&f->service)==TR2_OK);
        assert(live_configuration_active(&f->service,&active));
        if(which==0u) active.generation=UINT32_MAX;else active.revision_counter=UINT32_MAX;
        assert(tr2_configuration_record_encode_live(&active,record,140u)==TR2_OK);
        raw_change(f,0u,record,140u);boot(f);
        assert(live_configuration_recover(&f->service)==TR2_OK);writes=f->physical.writes;
        assert(live_configuration_apply(&f->service,&q,false)==TR2_ERROR_INVALID_STATE);
        assert(writes==f->physical.writes);
    }
    /* Ownership absent and unknown root: neither case may write/format. */
    memcpy(f->physical.bytes,baseline,262144u);boot(f);
    assert(live_configuration_recover(&f->service)==TR2_OK);
    f->live.check_write_ownership=NULL;
    {size_t writes=f->physical.writes;
     assert(live_configuration_apply(&f->service,&q,false)!=TR2_OK);
     assert(writes==f->physical.writes);}
    assert(transactional_image_media_format_empty(&f->live)==TR2_ERROR_INVALID_STATE);
    free(baseline);free(f);puts("Corruption/unknown schema/conflicts/UID/provenance/control boundary/generation limits: PASS");
}
static void commit_read_failures(void)
{
    Fixture *f=fixture();uint8_t *baseline=malloc(262144u);
    PreparedConfiguration a=prepared(vector_1_prepared,2u),b=prepared(vector_2_prepared,3u);
    ActiveConfigurationSnapshot active;unsigned phase;
    assert(baseline);assert(live_configuration_migrate(&f->service)==TR2_OK);
    assert(live_configuration_apply(&f->service,&a,false)==TR2_OK);
    memcpy(baseline,f->physical.bytes,262144u);
    for(phase=1u;phase<=3u;phase++) {
        memcpy(f->physical.bytes,baseline,262144u);boot(f);
        f->physical.writes=0u;f->physical.fail_read_after_write=phase;
        assert(live_configuration_apply(&f->service,&b,false)==TR2_ERROR_STORAGE);
        assert(!live_configuration_active(&f->service,&active));
        f->physical.fail_read=false;f->physical.fail_read_after_write=0u;boot(f);
        assert(live_configuration_recover(&f->service)==TR2_OK);
        assert(live_configuration_active(&f->service,&active));
        assert(active.config_id==(phase==3u?3u:2u));
        assert(memcmp(baseline,f->physical.bytes,122880u)==0);
    }
    free(baseline);free(f);puts("Read failures at commit barriers, uncertain publication recovered without false authority: PASS");
}
int main(void)
{
    sha_vectors();validator_and_codecs();nominal_and_guards();
    corruption_and_limits();commit_read_failures();power_cuts(true);power_cuts(false);return 0;
}
