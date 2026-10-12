/* Copyright (c) 2020 STMicroelectronics. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause (vendor/st/LICENSE.examples)
 * Modified for M015-K: bounded platform adaptation. See AUDIT.md. */
#include "bench.h"
#include <string.h>
#define TRY(call) do { if ((call) != 0) { result=BENCH_IO; goto done; } } while (0)
static int controls(stmdev_ctx_t *c, unsigned fifo, BenchResult *o)
{
  uint8_t bdu, inc, ts;
  iis3dwb_fs_xl_t fs; iis3dwb_odr_xl_t odr; iis3dwb_sim_t wire;
  if (iis3dwb_read_reg(c,7,o->controls,4) ||
      iis3dwb_read_reg(c,15,o->controls+4,10) ||
      iis3dwb_block_data_update_get(c,&bdu) ||
      iis3dwb_auto_increment_get(c,&inc) ||
      iis3dwb_xl_full_scale_get(c,&fs) ||
      iis3dwb_xl_data_rate_get(c,&odr) ||
      iis3dwb_spi_mode_get(c,&wire)) return BENCH_IO;
  if (o->controls[4]!=IIS3DWB_ID || !bdu || !inc ||
      wire!=IIS3DWB_SPI_4_WIRE || odr!=IIS3DWB_XL_ODR_26k7Hz ||
      fs!=(fifo?IIS3DWB_8g:IIS3DWB_2g)) return BENCH_CONFIG;
  if (fifo) {
    uint16_t wm; iis3dwb_bdr_xl_t rate; iis3dwb_fifo_mode_t mode;
    iis3dwb_fifo_timestamp_batch_t dec;
    if (iis3dwb_fifo_watermark_get(c,&wm) || iis3dwb_fifo_xl_batch_get(c,&rate) ||
        iis3dwb_fifo_mode_get(c,&mode) || iis3dwb_timestamp_get(c,&ts) ||
        iis3dwb_fifo_timestamp_batch_get(c,&dec)) return BENCH_IO;
    o->st_batch=(uint32_t)rate;
    /* Upstream getter switches on 0x10 instead of the setter's 0x0A.
     * Preserve it as telemetry; enforce actual BDR bits without changing ST. */
    if (wm!=256 || (o->controls[2]&0x0F)!=IIS3DWB_XL_BATCHED_AT_26k7Hz ||
        mode!=IIS3DWB_STREAM_MODE || ts!=1 || dec!=(BENCH_FIFO_DECIMATION?IIS3DWB_DEC_8:IIS3DWB_NO_DECIMATION))
      return BENCH_CONFIG;
  } else {
    iis3dwb_filt_xl_en_t filter;
    if (iis3dwb_xl_filt_path_on_out_get(c,&filter)) return BENCH_IO;
    if (filter!=IIS3DWB_LP_ODR_DIV_100) return BENCH_CONFIG;
  }
  return BENCH_OK;
}
int bench_run(stmdev_ctx_t *c, const BenchClock *clock,
              unsigned fifo, unsigned status10, BenchResult *o)
{
  uint8_t who=0,rst=1,ready=0; uint32_t start; int result=BENCH_OK, changed=0;
  memset(o,0,sizeof(*o));o->status10=status10;o->stage=1;
  clock->delay(10); /* BOOT_TIME in both official examples */
  TRY(iis3dwb_device_id_get(c,&who));o->who=who;
  if(who!=IIS3DWB_ID){result=BENCH_WHO;goto done;}
  if(fifo==2U)goto done; /* Separate read-only WHO image for first bring-up. */
  o->stage=2;changed=1;TRY(iis3dwb_reset_set(c,PROPERTY_ENABLE));start=clock->tick();
  do {
    TRY(iis3dwb_reset_get(c,&rst));
    if(clock->stop()){result=BENCH_STOP;goto done;}
    if(clock->tick()-start>=100){result=BENCH_RESET_TIMEOUT;goto done;}
    if(rst)clock->delay(1);
  } while(rst);
  o->stage=3;TRY(iis3dwb_block_data_update_set(c,PROPERTY_ENABLE));
  /* Reset default IF_INC=1/SIM=0 checked, no extra configuration fallback. */
  if(!fifo) {
    TRY(iis3dwb_xl_data_rate_set(c,IIS3DWB_XL_ODR_26k7Hz));
    TRY(iis3dwb_xl_full_scale_set(c,IIS3DWB_2g));
    TRY(iis3dwb_xl_filt_path_on_out_set(c,IIS3DWB_LP_ODR_DIV_100));
  } else {
    TRY(iis3dwb_xl_full_scale_set(c,IIS3DWB_8g));
    TRY(iis3dwb_fifo_watermark_set(c,256));
    if(!BENCH_FIFO_BDR_LAST)TRY(iis3dwb_fifo_xl_batch_set(c,IIS3DWB_XL_BATCHED_AT_26k7Hz));
    TRY(iis3dwb_fifo_mode_set(c,IIS3DWB_STREAM_MODE));
    TRY(iis3dwb_xl_data_rate_set(c,IIS3DWB_XL_ODR_26k7Hz));
    TRY(iis3dwb_fifo_timestamp_batch_set(c,BENCH_FIFO_DECIMATION?IIS3DWB_DEC_8:IIS3DWB_NO_DECIMATION));
    TRY(iis3dwb_timestamp_set(c,PROPERTY_ENABLE));
    if(BENCH_FIFO_BDR_LAST)TRY(iis3dwb_fifo_xl_batch_set(c,IIS3DWB_XL_BATCHED_AT_26k7Hz));
  }
  result=controls(c,fifo,o);if(result)goto done;
  o->stage=4;start=clock->tick();
  while(o->count<(fifo?BENCH_CAPTURE_ENTRIES:64U)) {
    if(clock->stop()){result=BENCH_STOP;goto done;}
    if(clock->tick()-start>=500){result=BENCH_TIMEOUT;goto done;}
    if(!fifo) {
      TRY(iis3dwb_xl_flag_data_ready_get(c,&ready));
      if(ready){TRY(iis3dwb_acceleration_raw_get(c,o->xyz[o->count]));o->count++;o->xyz_count++;}
      TRY(iis3dwb_temp_flag_data_ready_get(c,&ready));
      if(ready)TRY(iis3dwb_temperature_raw_get(c,&o->temperature));
    } else {
      uint8_t raw[2]={0}; iis3dwb_fifo_status_t st={0}; uint16_t n;
      if(o->polls++>=400U){result=BENCH_CAPACITY;goto done;}
      /* Official grouped status getter, preserved without modifying its mask. */
      TRY(iis3dwb_fifo_status_get(c,&st));
      /* A second raw observation is explicit, not substituted into the first. */
      TRY(iis3dwb_read_reg(c,IIS3DWB_FIFO_STATUS1,raw,2));
      o->st_level=st.fifo_level;o->raw_level=(raw[0]|((uint16_t)raw[1]<<8))&0x3FF;
      o->flags=raw[1];
      if(st.fifo_ovr || st.fifo_full || (raw[1]&0x68)){result=BENCH_OVERFLOW;goto done;}
      if(o->raw_level>512){result=BENCH_CAPACITY;goto done;}
      if(!status10 && st.fifo_th && st.fifo_level<256){result=BENCH_ST_LEVEL;goto done;}
      n=status10?(uint16_t)o->raw_level:st.fifo_level;
      if((status10?(raw[1]&0x80):st.fifo_th) && n>=256) {
        /* Official API; segmentation avoids SPI3 TSIZE and FIFO array overflow. */
        n=n>BENCH_FIFO_BURST?BENCH_FIFO_BURST:n;
        if(n>BENCH_CAPTURE_ENTRIES-o->count)n=(uint16_t)(BENCH_CAPTURE_ENTRIES-o->count);
        TRY(iis3dwb_fifo_out_multi_raw_get(c,&o->words[o->count],n));
        /* Retain rejected payload as received; count includes all received words. */
        unsigned first=o->count;o->count+=n;
        result=controls(c,1,o);if(result)goto done;
        for(unsigned i=first;i<o->count;i++) {
          unsigned tag=o->words[i].tag>>3;
          if(tag==IIS3DWB_XL_TAG)o->xyz_count++;
          else if(tag==IIS3DWB_TIMESTAMP_TAG)o->timestamp_count++;
          else {result=BENCH_TAG;goto done;}
        }
      }
      /* Bound repetitive level polls without changing sensor configuration. */
      clock->delay(1);
    }
  }
done:
  o->result=(uint32_t)result;o->stage=5;
  /* Dedicated bench only: fail closed, best-effort stop, preserve first error. */
  int cleanup=0;
  if(changed) {
    if(clock->cleanup_begin)clock->cleanup_begin();
    cleanup=iis3dwb_xl_data_rate_set(c,IIS3DWB_XL_ODR_OFF);
    if(iis3dwb_fifo_mode_set(c,IIS3DWB_BYPASS_MODE))cleanup=-1;
    if(iis3dwb_timestamp_set(c,PROPERTY_DISABLE))cleanup=-1;
    if(iis3dwb_read_reg(c,IIS3DWB_CTRL1_XL,o->cleanup_controls,1) ||
       iis3dwb_read_reg(c,IIS3DWB_FIFO_CTRL4,o->cleanup_controls+1,1) ||
       iis3dwb_read_reg(c,IIS3DWB_CTRL10_C,o->cleanup_controls+2,1))cleanup=-1;
    if((o->cleanup_controls[0]&0xE0U) || (o->cleanup_controls[1]&7U) ||
       (o->cleanup_controls[2]&0x20U))cleanup=-1;
  }
  o->cleanup_result=cleanup?BENCH_IO:BENCH_OK;o->stage=6;
  return result;
}
