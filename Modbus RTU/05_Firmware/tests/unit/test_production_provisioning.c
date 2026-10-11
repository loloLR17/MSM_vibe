#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "tr2/application/production_provisioning.h"

/* Host-only image fixture. This native struct is NOT a proposed persistent
 * board format/allocation, and is never linked into the STM32 binding. */
typedef struct {
    uint8_t bytes[TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE];
    unsigned writes;
    unsigned fail_write;
    bool fail_read;
} Physical;
static Tr2Result rd(void *ctx, uint32_t offset, void *buffer, size_t size)
{
    Physical *p = ctx;
    if (p->fail_read || offset > sizeof(p->bytes) || size > sizeof(p->bytes)-offset)
        return TR2_ERROR_STORAGE;
    memcpy(buffer, p->bytes+offset, size); return TR2_OK;
}
static Tr2Result wr(void *ctx, uint32_t offset, const void *buffer, size_t size)
{
    Physical *p = ctx;
    if (offset > sizeof(p->bytes) || size > sizeof(p->bytes)-offset) return TR2_ERROR_STORAGE;
    ++p->writes;
    if (p->writes == p->fail_write) {
        memcpy(p->bytes+offset, buffer, size/2u); return TR2_ERROR_STORAGE;
    }
    memcpy(p->bytes+offset, buffer, size); return TR2_OK;
}
static Tr2Result inspect(void *context, const uint8_t *image, size_t size,
                         ProductionProvisioningIdentity *identity)
{
    (void)context;
    if (size != TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) return TR2_ERROR_INVALID_ARGUMENT;
    /* This test image only has the explicitly simulated identity + a data byte.
     * A board implementation must validate its complete approved layout. */
    memcpy(identity, image, sizeof(*identity)); return TR2_OK;
}
static void init(Physical *p, TransactionalImageMedia *media, uint8_t *candidate)
{
    TransactionalImagePhysicalStorage physical = {p, rd, wr};
    TransactionalImageGeometry geometry = transactional_image_geometry_qualification_profile();
    assert(transactional_image_media_init(media, &physical, &geometry, candidate,
        TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE) == TR2_OK);
}
static ProductionProvisioningIdentity identity(void)
{
    ProductionProvisioningIdentity id = {0};
    id.hardware_uid[0]=0x12345678u; id.hardware_uid[1]=0xABCDEF01u; id.hardware_uid[2]=0xFEDCBA98u;
    id.functional.device_id=0x76543210u; id.functional.protocol_version=7u;
    id.functional.hardware_version=2u; id.functional.firmware_version_major=3u;
    id.functional.generation=1u; id.functional.device_capabilities=15u;
    memcpy(id.functional.serial_number,"HOST-FIXTURE",12u);
    memcpy(id.functional.manufacturer,"TEST",4u); id.unit_id=23u;
    return id;
}
static void install_fixture(Physical *p, TransactionalImageMedia *media)
{
    PersistentMedia *interface = transactional_image_media_interface(media);
    ProductionProvisioningIdentity id = identity();
    assert(transactional_image_media_format_empty(media) == TR2_OK); /* explicit host setup */
    assert(interface->write(interface->context,0u,&id,sizeof(id)) == TR2_OK);
    assert(interface->commit(interface->context) == TR2_OK);
    p->writes=0u;
}
static ProductionProvisioningRequest request(uint8_t *image, uint64_t generation)
{
    ProductionProvisioningIdentity id = identity();
    ProductionProvisioningRequest r = {0};
    memcpy(r.target_uid,id.hardware_uid,sizeof(r.target_uid));
    r.supported_protocol_version=7u; r.expected_generation=generation;
    r.exclusive_ownership=true; r.image=image;
    r.image_size=TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE; r.inspect_image=inspect;
    return r;
}
static void test_probe_and_revocation(void)
{
    Physical *p=calloc(1,sizeof(*p)); uint8_t *candidate=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    TransactionalImageMedia media; TransactionalImageRecoveryResult recovery;
    assert(p && candidate);
    assert(production_provisioning_probe(NULL,&recovery)==TR2_ERROR_NOT_AVAILABLE);
    assert(recovery.status==TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE);
    memset(p->bytes,0xFF,sizeof(p->bytes)); init(p,&media,candidate);
    assert(production_provisioning_probe(&media,&recovery)==TR2_ERROR_NOT_AVAILABLE);
    assert(recovery.status==TRANSACTIONAL_IMAGE_RECOVERY_EMPTY && p->writes==0);
    install_fixture(p,&media);
    assert(production_provisioning_probe(&media,&recovery)==TR2_OK);
    /* A corrupt older peer is recoverable only because generation2 survives. */
    p->bytes[TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE]=0x12u;
    assert(production_provisioning_probe(&media,&recovery)==TR2_OK);
    assert(recovery.generation==2u && p->writes==0);
    p->fail_read=true;
    assert(production_provisioning_probe(&media,&recovery)==TR2_ERROR_UNAVAILABLE);
    assert(!media.recovered && media.recovery_required && p->writes==0);
    assert(media.interface.read(&media,0u,candidate,1u)==TR2_ERROR_INVALID_STATE);
    assert(media.interface.write(&media,0u,candidate,1u)==TR2_ERROR_INVALID_STATE);
    assert(media.interface.commit(&media)==TR2_ERROR_INVALID_STATE);
    p->fail_read=false;
    assert(production_provisioning_probe(&media,&recovery)==TR2_OK);
    memset(p->bytes,0xFF,sizeof(p->bytes));
    assert(production_provisioning_probe(&media,&recovery)==TR2_ERROR_NOT_AVAILABLE);
    assert(!media.recovered && p->writes==0);
    p->bytes[0]=0x12u;
    assert(production_provisioning_probe(&media,&recovery)==TR2_ERROR_CORRUPTED);
    assert(recovery.status==TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED);
    memcpy(p->bytes,"TR2M",4u); p->bytes[4]=0xFFu; p->bytes[5]=0xFFu;
    assert(production_provisioning_probe(&media,&recovery)==TR2_ERROR_UNSUPPORTED);
    assert(!media.recovered && p->writes==0);
    free(candidate);free(p);
}
static void test_plan_idempotence_and_rejections(void)
{
    Physical *p=calloc(1,sizeof(*p)); uint8_t *candidate=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    uint8_t *image=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
    TransactionalImageMedia media; ProductionProvisioningResult result;
    ProductionProvisioningRequest r; ProductionProvisioningIdentity id;
    assert(p && candidate && image);memset(p->bytes,0xFF,sizeof(p->bytes));init(p,&media,candidate);
    memset(image,0,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);r=request(image,0u);
    assert(production_provisioning_apply(&media,&r,true,&result)==TR2_ERROR_NOT_AVAILABLE);
    assert(p->writes==0 && !result.identity_available);
    install_fixture(p,&media);memcpy(image,candidate,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);r=request(image,2u);
    assert(production_provisioning_plan(&media,&r,&result)==TR2_OK);
    assert(result.unchanged && result.identity_available && result.unit_id==23u);
    assert(result.identity.registers[0]==0x7654u && result.identity.registers[1]==0x3210u);
    assert(result.identity.registers[20]==0u && p->writes==0);
    assert(production_provisioning_apply(&media,&r,false,&result)==TR2_ERROR_NOT_AVAILABLE);
    assert(production_provisioning_apply(&media,&r,true,&result)==TR2_OK && p->writes==0);
    r.exclusive_ownership=false;
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_NOT_AVAILABLE);
    r.exclusive_ownership=true;r.inspect_image=NULL;
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_NOT_AVAILABLE);
    r.inspect_image=inspect;r.expected_generation=1u;
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_INVALID_STATE);
    r.expected_generation=2u;r.target_uid[2]^=1u;
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_INVALID_STATE);
    r.target_uid[2]^=1u;r.supported_protocol_version=8u;
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_UNSUPPORTED);
    r.supported_protocol_version=7u;
    id=identity();id.unit_id=0u;memcpy(image,&id,sizeof(id));
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_INVALID_ARGUMENT);
    id.unit_id=248u;memcpy(image,&id,sizeof(id));
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_INVALID_ARGUMENT);
    id=identity();id.functional.device_id++;memcpy(image,&id,sizeof(id));
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_INVALID_STATE);
    id=identity();id.functional.protocol_version++;memcpy(image,&id,sizeof(id));
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_UNSUPPORTED);
    id=identity();id.functional.device_capabilities|=0x10u;memcpy(image,&id,sizeof(id));
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_INVALID_ARGUMENT);
    assert(p->writes==0 && !result.identity_available);
    r.image=candidate;
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_INVALID_ARGUMENT);
    r.image=candidate+1u;
    assert(production_provisioning_plan(&media,&r,&result)==TR2_ERROR_INVALID_ARGUMENT);
    free(image);free(candidate);free(p);
}
static void test_commit_readback_and_torn_transactions(void)
{
    unsigned cut;
    for(cut=0;cut<=3u;++cut) {
        Physical *p=calloc(1,sizeof(*p)); uint8_t *candidate=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
        uint8_t *image=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
        uint8_t *reboot_candidate=malloc(TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);
        TransactionalImageMedia media,reboot; TransactionalImageRecoveryResult recovery;
        ProductionProvisioningResult result;ProductionProvisioningRequest r;
        assert(p && candidate && image && reboot_candidate);
        memset(p->bytes,0xFF,sizeof(p->bytes));init(p,&media,candidate);install_fixture(p,&media);
        memcpy(image,candidate,TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE);image[1000u]=0xA5u;r=request(image,2u);
        p->fail_write=cut;
        if(cut==0u) {
            assert(production_provisioning_apply(&media,&r,true,&result)==TR2_OK);
            assert(p->writes==3u && result.write_attempted && result.identity_available);
            assert(result.recovery.generation==3u);
            r.expected_generation=3u;
            assert(production_provisioning_apply(&media,&r,true,&result)==TR2_OK);
            assert(result.unchanged && !result.write_attempted && p->writes==3u);
        } else {
            assert(production_provisioning_apply(&media,&r,true,&result)==TR2_ERROR_STORAGE);
            assert(result.write_attempted && !result.identity_available && p->writes==cut);
        }
        p->fail_write=0u;init(p,&reboot,reboot_candidate);
        assert(production_provisioning_probe(&reboot,&recovery)==TR2_OK);
        assert(recovery.generation==(cut==0u?3u:2u));
        assert(reboot_candidate[1000u]==(cut==0u?0xA5u:0u));
        assert(reboot_candidate[0]==image[0]);
        free(reboot_candidate);free(image);free(candidate);free(p);
    }
}
int main(void)
{
    test_probe_and_revocation();
    test_plan_idempotence_and_rejections();
    test_commit_readback_and_torn_transactions();
    return 0;
}
