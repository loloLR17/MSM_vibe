/* Offline rehearsal on a COPY of an actual complete FRAM backup. No hardware. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tr2_generated_commissioning.h"
#include "tr2/persistence/configuration_store.h"
static uint8_t physical_bytes[262144],candidate[51914],desired[51914],original[51818];
static unsigned writes;
static Tr2Result read_bytes(void *ctx,uint32_t offset,void *buffer,size_t size)
{(void)ctx;if(offset>262144||size>262144-offset)return TR2_ERROR_STORAGE;memcpy(buffer,physical_bytes+offset,size);return TR2_OK;}
static Tr2Result write_bytes(void *ctx,uint32_t offset,const void *buffer,size_t size)
{(void)ctx;if(offset>262144||size>262144-offset)return TR2_ERROR_STORAGE;memcpy(physical_bytes+offset,buffer,size);++writes;return TR2_OK;}
#define REQUIRE(expr) do{if(!(expr)){fprintf(stderr,"Failed at line %d: %s\n",__LINE__,#expr);return 1;}}while(0)
int main(int argc,char **argv)
{
    FILE *input;TransactionalImageMedia media;ProductionProvisioningResult out;
    TransactionalImagePhysicalStorage physical={NULL,read_bytes,write_bytes};
    TransactionalImageGeometry geometry={262144,0,4096,8192,65536,57344};
    ProductionCommissioningAuthorities authority={{true,UINT32_MAX},&tr2_board_assignment,1};
    ProductionCommissioningConfigurationRequest q={0};PersistentIdentityProvisioningRequest r={0};
    ConfigurationStore store={0};PersistentStorageCore core={0};ConfigurationRecoveryResult recovery;
    ConfigurationValidationEnvironment unavailable={false,0};
    REQUIRE(argc==3);input=fopen(argv[1],"rb");REQUIRE(input!=NULL);
    REQUIRE(fread(physical_bytes,1,sizeof(physical_bytes),input)==sizeof(physical_bytes));REQUIRE(fgetc(input)==EOF);fclose(input);
    REQUIRE(transactional_image_media_init_v2(&media,&physical,&geometry,candidate,sizeof(candidate))==TR2_OK);
    REQUIRE(production_provisioning_probe(&media,&out.recovery)==TR2_OK&&out.recovery.generation==10);
    memcpy(original,candidate,51818);q.authorities=&authority;q.configuration=&tr2_board_configuration;
    memcpy(q.observed_uid,tr2_board_assignment.hardware_uid,12);memcpy(q.target_uid,q.observed_uid,12);
    q.device_id=tr2_board_assignment.device_id;q.exclusive_ownership=true;q.expected_generation=10;
    REQUIRE(production_commissioning_install_configuration(&media,&q,false,&out)==TR2_ERROR_NOT_AVAILABLE&&writes==0);
    q.observed_uid[2]^=1;REQUIRE(production_commissioning_install_configuration(&media,&q,true,&out)==TR2_ERROR_INVALID_STATE&&writes==0);q.observed_uid[2]^=1;
    REQUIRE(production_commissioning_install_configuration(&media,&q,true,&out)==TR2_OK&&out.recovery.generation==11&&writes==3);
    REQUIRE(candidate[0]==0xAC&&memcmp(candidate,original,140)==0&&memcmp(candidate+280,original+280,51818-280)==0);
    memcpy(desired,candidate,51818);REQUIRE(persistent_identity_encode(&tr2_board_identity,desired+51818)==TR2_OK);
    r.context=&authority;r.attribution=production_commissioning_check_attribution;r.validate_prefix=production_commissioning_inspect_prefix;
    memcpy(r.target_uid,tr2_board_assignment.hardware_uid,12);r.supported_protocol_version=1;r.expected_generation=11;r.exclusive_ownership=true;r.image=desired;r.image_size=sizeof(desired);
    REQUIRE(persistent_identity_provision(&media,&r,true,&out)==TR2_OK&&out.recovery.generation==12&&writes==6);
    REQUIRE(memcmp(out.identity.registers,tr2_board_expected_b0,sizeof(tr2_board_expected_b0))==0);
    REQUIRE(persistent_storage_core_init(&core,transactional_image_media_interface(&media))==TR2_OK);
    REQUIRE(configuration_store_init(&store,&core)==TR2_OK);
    REQUIRE(configuration_store_recover(&store,&unavailable,&recovery)==TR2_OK&&recovery.status==CONFIGURATION_RECOVERY_UNAVAILABLE);
    input=fopen(argv[2],"wb");REQUIRE(input!=NULL);REQUIRE(fwrite(physical_bytes,1,sizeof(physical_bytes),input)==sizeof(physical_bytes));REQUIRE(fclose(input)==0);
    puts("PASS: actual backup copy generation10->11->12; six bounded writes; unchanged other prefix; expected B0; config UNAVAILABLE without storage authority.");return 0;
}
