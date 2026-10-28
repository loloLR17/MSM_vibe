#include "tr2/application/live_metrology.h"
#include "tr2/domain/configuration/configuration_validator.h"
#include "tr2/domain/supervision/indicator_calculator.h"
#include "tr2/platform/iis3dwb_units.h"
#include <limits.h>
#include <string.h>

static uint32_t increment(uint32_t n) { return n == UINT32_MAX ? n : n + 1u; }
static uint16_t clear_threshold(uint16_t t, uint16_t h) { return t > h ? (uint16_t)(t-h) : 0; }
static void cancel_timers(LiveMetrology *s)
{
    for(unsigned i=0;i<4;i++) s->alarms[i].pending=false;
}
void live_metrology_init(LiveMetrology *s)
{
    if(s) { memset(s,0,sizeof(*s)); s->boot=true; }
}
void live_metrology_invalidate(LiveMetrology *s, bool calc, bool authority_lost)
{
    if(!s) return;
    live_dynamic_invalidate(&s->dynamic);
    s->boot=false; s->value_valid=false; s->error=true; s->calc_error|=calc;
    s->running=false; s->window_active=false;
    if(authority_lost) s->authority=false;
    cancel_timers(s);
    if(s->safe_stop) s->safe_stop(s->stop_context);
}
Tr2Result live_metrology_start(LiveMetrology *s,uint64_t run,const ActiveConfigurationSnapshot *c)
{
    PreparedConfiguration p={0};
    if(!s || !c || !run || !c->generation) return TR2_ERROR_INVALID_ARGUMENT;
    p.config_id=c->config_id; p.payload=c->payload;
    if(s->running || run<=s->run_id ||
       configuration_validate_live(&p,NULL).status!=CONFIGURATION_VALIDATION_VALID)
        return TR2_ERROR_INVALID_STATE;
    live_dynamic_start(&s->dynamic,run,c);
    s->active=*c; s->run_id=run; s->next_index=0;
    s->boot=false; s->authority=true; s->running=true; s->value_valid=false;
    s->error=s->calc_error=s->window_active=false; cancel_timers(s);
    return TR2_OK;
}
void live_metrology_stop(LiveMetrology *s)
{
    if(!s) return;
    live_dynamic_stop(&s->dynamic);
    s->boot=false; s->running=false; s->window_active=false; cancel_timers(s);
}
static unsigned alarm_bits(const LiveMetrology *s,bool exceed)
{
    unsigned bits=0;
    for(unsigned i=0;i<4;i++)
        if(s->alarms[i].alarm || (exceed && s->alarms[i].warning)) bits|=1u<<i;
    return bits;
}
static void evaluate(LiveMetrology *s,const VibrationIndicators *v,uint64_t now)
{
    const ConfigurationPayload *c=&s->active.payload;
    uint32_t rms[4]={v->rms_global_mg,v->rms_x_mg,v->rms_y_mg,v->rms_z_mg};
    uint32_t peak[4]={v->peak_global_mg,v->peak_x_mg,v->peak_y_mg,v->peak_z_mg};
    unsigned old_alarm=alarm_bits(s,false),old_exceed=alarm_bits(s,true);
    for(unsigned i=0;i<4;i++) {
        LiveMetricAlarm *a=&s->alarms[i];
        if(i && !(c->axes_enable_mask & (1u<<(i-1)))) {
            a->alarm=a->warning=a->pending=false; continue;
        }
        if(rms[i]>c->rms_warn_threshold_mg || peak[i]>c->peak_warn_threshold_mg)
            a->warning=true;
        else if(rms[i]<=clear_threshold(c->rms_warn_threshold_mg,c->threshold_hysteresis_mg) &&
                peak[i]<=clear_threshold(c->peak_warn_threshold_mg,c->threshold_hysteresis_mg))
            a->warning=false;
        if(a->alarm) {
            if(rms[i]<=clear_threshold(c->rms_alarm_threshold_mg,c->threshold_hysteresis_mg) &&
               peak[i]<=clear_threshold(c->peak_alarm_threshold_mg,c->threshold_hysteresis_mg))
                a->alarm=false;
            a->pending=false;
        } else if(rms[i]>c->rms_alarm_threshold_mg || peak[i]>c->peak_alarm_threshold_mg) {
            if(!a->pending) { a->pending=true; a->candidate_ms=now; }
            if(now-a->candidate_ms>=c->alarm_hold_time_ms) { a->alarm=true; a->pending=false; }
        } else a->pending=false;
    }
    unsigned alarms=alarm_bits(s,false),exceeds=alarm_bits(s,true);
    if(alarms & ~old_alarm) s->latched=true;
    if(!old_alarm && alarms) s->alarm_count=increment(s->alarm_count);
    if(!old_exceed && exceeds) s->exceed_count=increment(s->exceed_count);
}
static bool coherent(const LiveMetrology *s)
{
    return s->authority && s->has_value && s->value.configuration.config_id==s->active.config_id &&
        s->value.configuration.generation==s->active.generation;
}
static bool fresh(const LiveMetrology *s,uint64_t now)
{
    return s->value_valid && coherent(s) && s->running &&
        now-s->value.value_monotonic_ms <= 2u*(uint64_t)s->active.payload.indicator_period_ms;
}
static Tr2Result complete(LiveMetrology *s,uint64_t now,bool utc_valid,uint64_t utc)
{
    VibrationIndicators v; SupervisionSnapshot out={0};
    s->window.end_monotonic_ms=now; s->window.complete=true;
    if(vibration_indicators_from_window(&s->window,&v)!=TR2_OK) {
        live_metrology_invalidate(s,true,false); return TR2_ERROR_INVALID_STATE;
    }
    if(s->has_value && now-s->value.value_monotonic_ms >=
       2u*(uint64_t)s->active.payload.indicator_period_ms) cancel_timers(s);
    evaluate(s,&v,now);
    out.configuration=s->window.configuration;
    s->sequence=increment(s->sequence); out.calculation_sequence=s->sequence;
    out.window_duration_ms=v.window_duration_ms; out.valid_sample_count=v.valid_sample_count;
    out.rms_global_mg=v.rms_global_mg; out.peak_global_mg=v.peak_global_mg;
    out.rms_x_mg=v.rms_x_mg; out.rms_y_mg=v.rms_y_mg; out.rms_z_mg=v.rms_z_mg;
    out.peak_x_mg=v.peak_x_mg; out.peak_y_mg=v.peak_y_mg; out.peak_z_mg=v.peak_z_mg;
    out.values_available=out.window_complete=true; out.value_monotonic_ms=now;
    out.civil_timestamp_available=utc_valid && utc<=UINT32_MAX;
    out.civil_timestamp=out.civil_timestamp_available?(uint32_t)utc:0;
    s->value=out; s->has_value=s->value_valid=true; s->window_active=false;
    return TR2_OK;
}
Tr2Result live_metrology_sample(LiveMetrology *s,uint64_t run,uint32_t id,uint32_t gen,
    uint64_t index,const int16_t xyz[3],uint64_t now,bool utc_valid,uint64_t utc)
{
    if(!s || !xyz) return TR2_ERROR_INVALID_ARGUMENT;
    if(!s->running) return TR2_ERROR_INVALID_STATE;
    if(run!=s->run_id || id!=s->active.config_id || gen!=s->active.generation ||
       index!=s->next_index || index==UINT64_MAX ||
       (s->time_seen && now<s->last_ms)) {
        live_metrology_invalidate(s,false,false); return TR2_ERROR_INVALID_STATE;
    }
    /* Any physical saturation is rejected, even on an unselected logical axis:
     * same conservative integrity rule as the qualified shared FIFO parser. */
    for(unsigned i=0;i<3;i++) if(xyz[i]==INT16_MIN || xyz[i]==INT16_MAX) {
        live_metrology_invalidate(s,false,false); return TR2_ERROR_INVALID_STATE;
    }
    if(index==0) s->next_window_ms=now;
    s->time_seen=true; s->last_ms=now; s->next_index++;
    if(s->dynamic.enabled&&live_dynamic_sample(&s->dynamic,xyz,now)!=TR2_OK) {
        live_metrology_invalidate(s,true,false);return TR2_ERROR_INVALID_STATE;
    }
    if(!s->window_active) {
        if(now<s->next_window_ms) return TR2_OK;
        if(now>UINT64_MAX-s->active.payload.indicator_period_ms) {
            live_metrology_invalidate(s,false,false); return TR2_ERROR_INVALID_STATE;
        }
        /* No catch-up loop or overlapping window. A late period starts at the
         * first available observed sample, without inventing missing windows. */
        s->next_window_ms=now+s->active.payload.indicator_period_ms;
        memset(&s->window,0,sizeof(s->window));
        s->window.configuration.generation=gen; s->window.configuration.config_id=id;
        s->window.configuration.revision_counter=s->active.revision_counter;
        s->window.configuration.payload=s->active.payload;
        s->window.start_monotonic_ms=now; s->window_active=true;
    }
    VibrationSample sample={0}; int32_t mg[3]={0};
    for(unsigned i=0;i<3;i++) if(s->active.payload.axes_enable_mask&(1u<<i))
        mg[i]=tr2_iis3dwb_raw_to_mg(xyz[i],s->active.payload.full_scale_code);
    sample.x_mg=mg[0]; sample.y_mg=mg[1]; sample.z_mg=mg[2]; sample.valid=true;
    if(!acquisition_window_accumulate_valid_sample(&s->window,&sample)) {
        live_metrology_invalidate(s,true,false); return TR2_ERROR_INVALID_STATE;
    }
    s->window.valid_sample_count++; s->window.acquired_sample_count++;
    if(s->window.valid_sample_count==s->active.payload.window_size_samples)
        return complete(s,now,utc_valid,utc);
    return TR2_OK;
}
static unsigned dominant(const LiveMetrology *s)
{
    uint32_t rms[3]={s->value.rms_x_mg,s->value.rms_y_mg,s->value.rms_z_mg},best=0;
    unsigned axis=0;
    for(unsigned i=0;i<3;i++) if(s->value.configuration.payload.axes_enable_mask&(1u<<i)) {
        if(!axis || rms[i]>best) { best=rms[i]; axis=i+1; }
        else if(rms[i]==best) axis=4;
    }
    return axis;
}
static void u32(uint16_t *r,unsigned at,uint32_t value)
{ r[at]=(uint16_t)(value>>16); r[at+1]=(uint16_t)value; }
Tr2Result live_metrology_project_b3(LiveMetrology *s,uint64_t now,ModbusBlock3Image *out)
{
    if(!s || !out) return TR2_ERROR_INVALID_ARGUMENT;
    if((s->read_time_seen && now<s->last_read_ms) ||
       (s->has_value && now<s->value.value_monotonic_ms))
        live_metrology_invalidate(s,false,false);
    else { s->last_read_ms=now; s->read_time_seen=true; }
    memset(out,0,sizeof(*out));
    if(s->has_value) {
        SupervisionSnapshot v=s->value;
        uint64_t age=now>=v.value_monotonic_ms?now-v.value_monotonic_ms:0;
        v.value_age_available=true; v.value_age_ms=age>UINT32_MAX?UINT32_MAX:(uint32_t)age;
        (void)modbus_project_b3(&v,out);
    }
    bool valid=s->value_valid && coherent(s),f=valid && fresh(s,now);
    bool held=s->has_value && !f, time=s->has_value && s->value.civil_timestamp_available;
    unsigned alarm=alarm_bits(s,false),exceed=alarm_bits(s,true);
    if(!f) cancel_timers(s);
    uint16_t flags=0;
    if(!s->boot) {
        if(s->authority) flags|=1;
        if(valid) flags|=2;
        if(f) flags|=4;
        if(s->has_value) flags|=8|16|32;
        if(time) flags|=64;
        if(coherent(s)) flags|=128;
        if(held) flags|=512;
        if(held || (s->has_value && !time) || s->error) flags|=1024;
        if(s->calc_error) flags|=2048;
        if(!s->running) flags|=4096;
    }
    out->registers[1]=flags;
    out->registers[0]=s->boot?0:s->error?5:!s->running?1:!s->has_value?2:!f||!time?4:3;
    out->registers[3]=!valid||!f?0:alarm?4:exceed?3:!time?2:1;
    out->registers[2]=(uint16_t)(alarm|(exceed<<4)|(s->latched?256u:0u));
    out->registers[30]=(uint16_t)(valid?dominant(s):0);
    for(unsigned i=0;i<4;i++) out->registers[31+i]=(exceed>>i)&1u;
    out->registers[35]=s->latched?1:0;
    u32(out->registers,36,s->exceed_count); u32(out->registers,38,s->alarm_count);
    return s->error?TR2_ERROR_INVALID_STATE:TR2_OK;
}
Tr2Result live_metrology_ack(LiveMetrology *s,uint64_t now)
{
    ModbusBlock3Image image;
    if(live_metrology_project_b3(s,now,&image)!=TR2_OK || !(image.registers[1]&4) || alarm_bits(s,false))
        return TR2_ERROR_INVALID_STATE;
    s->latched=false; return TR2_OK;
}
