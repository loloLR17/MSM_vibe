/* Offline binary replay, explicit little endian, no serial/hardware APIs. */
#include "tr2/domain/acquisition/fir_decimator10.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
void d10_dot_f32(const int16_t *,unsigned,float [3]);
void d10_dot_q31(const int16_t *,unsigned,int64_t [3]);
static int16_t rounded(double x)
{
    double q=nearbyint(x);
    if(q>32767)q=32767;
    if(q< -32768)q=-32768;
    return (int16_t)q;
}
int main(int argc,char **argv)
{
    if(argc!=5)return 2;
    FILE *input=fopen(argv[1],"rb"),*output=fopen(argv[2],"wb");
    if(!input||!output)return 3;
    const unsigned pattern[]={1,7,10,13,31,255,512,1024};
    unsigned chunk=(unsigned)strtoul(argv[4],NULL,10),step=0;
    if(chunk>1024)return 4;
    int candidate=strcmp(argv[3],"q15")==0?0:strcmp(argv[3],"q31")==0?1:strcmp(argv[3],"f32")==0?2:-1;
    if(candidate<0)return 4;
    FirDecimator10 s;fir_decimator10_init(&s);
    if(fir_decimator10_start(&s,1,0,(FirDecimator10Clock){80000,3,false})!=TR2_OK)return 5;
    uint64_t index=0,warmup=0,clipflags=0,railflags=0;
    for(;;) {
        FirDecimator10Input in[1024];FirDecimator10Output out[104];
        unsigned n=0,want=chunk?chunk:pattern[step++%8];
        for(;n<want;n++) {
            unsigned char bytes[6];size_t got=fread(bytes,1,6,input);
            if(!got)break;
            if(got!=6)return 6;
            memset(&in[n],0,sizeof(in[n]));in[n].valid=in[n].time_valid=true;
            in[n].source_index=index;in[n].time_us=index*75/2;index++;
            for(unsigned a=0;a<3;a++) {
                unsigned b=bytes[2*a]|((unsigned)bytes[2*a+1]<<8);
                in[n].xyz[a]=(int16_t)(b<=32767?(int)b:(int)b-65536);
            }
        }
        if(!n)break;
        size_t consumed=0,written=0;
        if(!candidate) {
            if(fir_decimator10_process(&s,in,n,out,104,&consumed,&written)!=TR2_OK||consumed!=n)return 7;
        }else {
            for(unsigned i=0;i<n;i++) {
                bool ready;
                if(fir_decimator10_push(&s,&in[i],&out[written],&ready)!=TR2_OK)return 7;
                if(ready) {
                    unsigned head=s.head?s.head-1:106;
                    float f[3];int64_t q[3];
                    if(candidate==1)d10_dot_q31(&s.history[0][0],head,q);
                    else d10_dot_f32(&s.history[0][0],head,f);
                    for(unsigned a=0;a<3;a++)out[written].xyz[a]=rounded(candidate==1?(double)q[a]/2147483648.0:(double)f[a]);
                    written++;
                }
            }
        }
        for(size_t i=0;i<written;i++) {
            if(out[i].source_index!=out[i].output_index*10||out[i].causal_time_us!=out[i].source_index*75/2)return 8;
            if(out[i].flags&TR2_D10_WARMUP)warmup++;
            if(out[i].flags&TR2_D10_OUTPUT_CLIP)clipflags++;
            if(out[i].flags&TR2_D10_SOURCE_RAIL)railflags++;
            unsigned char bytes[6];
            for(unsigned a=0;a<3;a++){uint16_t u=(uint16_t)out[i].xyz[a];bytes[2*a]=(unsigned char)u;bytes[2*a+1]=(unsigned char)(u>>8);}
            if(fwrite(bytes,1,6,output)!=6)return 9;
        }
    }
    if(ferror(input)||fclose(input)||fclose(output))return 10;
    fir_decimator10_stop(&s);
    printf("{\"source\":%"PRIu64",\"output\":%"PRIu64",\"warmup\":%"PRIu64",\"clipped\":%"PRIu64",\"rail_flags\":%"PRIu64",\"source_rails\":%"PRIu64",\"state_bytes\":%zu}\n",s.accepted,s.emitted,warmup,clipflags,railflags,s.source_rail_samples,sizeof(s));
    return 0;
}
