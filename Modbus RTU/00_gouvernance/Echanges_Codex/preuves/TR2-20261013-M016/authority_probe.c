#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "tr2/persistence/configuration_record.h"
#include "tr2/domain/configuration/configuration_validator.h"
int main(int argc, char **argv) {
    unsigned char bytes[140]; unsigned value; FILE *f;
    ActiveConfigurationSnapshot record;
    PreparedConfiguration p = {0};
    ConfigurationValidationEnvironment environments[] = {{false,0},{true,0},{true,1},{true,UINT32_MAX}};
    ConfigurationValidationStatus expected[] = {CONFIGURATION_VALIDATION_ENVIRONMENT_NOT_CHARACTERIZED, CONFIGURATION_VALIDATION_INVALID, CONFIGURATION_VALIDATION_VALID, CONFIGURATION_VALIDATION_VALID};
    const char *names[] = {"production_unknown", "known_zero_capacity", "hypothetical_one_MB", "structural_only_ceiling"};
    assert(argc==2); f=fopen(argv[1],"r"); assert(f);
    for(unsigned i=0;i<140;i++) { assert(fscanf(f,"%2x",&value)==1); bytes[i]=(unsigned char)value; }
    assert(fclose(f)==0);
    assert(tr2_configuration_record_decode(bytes,140,&record)==TR2_OK);
    assert(record.generation==1 && record.config_id==1 && record.revision_counter==1);
    assert(record.payload.full_scale_code==0 && record.payload.storage_mode==1 && record.payload.storage_limit_mb==1);
    p.generation=record.generation; p.config_id=record.config_id; p.payload=record.payload;
    printf("record CRC/decode OK; generation=%u config=%u revision=%u fs=%u storage_mode=%u storage_limit_mb=%u window=%u odr=%u\n",record.generation,record.config_id,record.revision_counter,record.payload.full_scale_code,record.payload.storage_mode,record.payload.storage_limit_mb,record.payload.window_size_samples,record.payload.sampling_frequency_hz);
    for(unsigned i=0;i<4;i++) {
        ValidatedConfiguration out, before; memset(&out,0xA5,sizeof(out)); before=out;
        ConfigurationValidationResult r=configuration_validate(&p,&environments[i],&out);
        assert(r.status==expected[i]);
        if(r.status!=CONFIGURATION_VALIDATION_VALID) assert(memcmp(&out,&before,sizeof(out))==0);
        printf("%s: status=%d %s\n",names[i],r.status,r.status==CONFIGURATION_VALIDATION_VALID?"VALID (hypothetical; no resource authority created)":"REFUSED; no validated output created");
    }
    p.payload.storage_mode=0;
    assert(configuration_validate(&p,&environments[2],NULL).status==CONFIGURATION_VALIDATION_INVALID);
    puts("storage_mode=0: INVALID even with hypothetical characterized capacity");
    p.payload.storage_mode=1; p.payload.storage_limit_mb=0;
    assert(configuration_validate(&p,&environments[2],NULL).status==CONFIGURATION_VALIDATION_INVALID);
    puts("storage_limit_mb=0: INVALID; no storage-free V1 configuration");
    return 0;
}
