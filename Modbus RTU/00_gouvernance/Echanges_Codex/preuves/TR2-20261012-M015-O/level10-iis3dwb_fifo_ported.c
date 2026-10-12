/*
 ******************************************************************************
 * @file    fifo.c
 * @author  Sensors Software Solution Team
 * @brief   This file how to configure compressed FIFO and to retrieve acc
 *          data. This sample use a fifo utility library tool
 *          for FIFO decompression.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/*
 * This example was developed using the following STMicroelectronics
 * evaluation boards:
 *
 * - STEVAL_MKI109D  +
 * - STEVAL_MKI109V3 + STEVAL-MKI208V1K
 *
 * Used interfaces:
 *
 * STEVAL_MKI109D     - Host side:   USB (Virtual COM)
 *                    - Sensor side: SPI(Default) / I2C(supported)
 *
 * STEVAL_MKI109V3    - Host side:   USB (Virtual COM)
 *                    - Sensor side: SPI(Default) / I2C(supported)
 *
 * If you need to run this example on a different hardware platform a
 * modification of the functions: `platform_write`, `platform_read`,
 * `tx_com` and 'platform_init' is required.
 *
 */

/* STMicroelectronics evaluation boards definition
 *
 * Please uncomment ONLY the evaluation boards in use.
 * If a different hardware is used please comment all
 * following target board and redefine yours.
 */

//#define STEVAL_MKI109V3  /* little endian */

/* ATTENTION: By default the driver is little endian. If you need switch
 *            to big endian please see "Endianness definitions" in the
 *            header file of the driver (_reg.h).
 */

#if defined(STEVAL_MKI109D)
/* MKI109D: Define communication interface */
#define SENSOR_BUS hspi1

/* MKI109D: Vdd and Vddio power supply values */
#define LSM6DSV16X_VDD 1.8f
#define LSM6DSV16X_VDDIO 1.8f

#elif defined(STEVAL_MKI109V3)
/* MKI109V3: Define communication interface */
#define SENSOR_BUS hspi2
/* MKI109V3: Vdd and Vddio power supply values */
#define PWM_3V3 915
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "iis3dwb_reg.h"
#include "port.h"

#if defined(STEVAL_MKI109D)
#include "board.h"
#include "usbd_cdc_if.h"

#elif defined(STEVAL_MKI109V3)
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#endif

/* Private macro -------------------------------------------------------------*/
/*
 * Select FIFO samples watermark, max value is 511
 * in FIFO are stored acc and timestamp samples
 */
#define BOOT_TIME         10
#define FIFO_WATERMARK    256

/* Private variables ---------------------------------------------------------*/
static uint8_t whoamI;
static uint8_t tx_buffer[1000];
static iis3dwb_fifo_out_raw_t fifo_data[512];

/* Private variables ---------------------------------------------------------*/
static int16_t *datax;
static int16_t *datay;
static int16_t *dataz;
static int32_t *ts;

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 *   WARNING:
 *   Functions declare in this section are defined at the end of this file
 *   and are strictly related to the hardware platform used.
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void tx_com( uint8_t *tx_buffer, uint16_t len );
static void platform_delay(uint32_t ms);
static void platform_init(void);


/* Main Example --------------------------------------------------------------*/

#if OFFICIAL_STATUS10
#define iis3dwb_fifo_status_get official_status10
#endif
void iis3dwb_fifo(void)
{
  iis3dwb_fifo_status_t fifo_status;
  stmdev_ctx_t dev_ctx;
  uint8_t rst;

  /* Uncomment to configure INT 1 */
  //iis3dwb_pin_int1_route_t int1_route;
  /* Uncomment to configure INT 2 */
  //iis3dwb_pin_int2_route_t int2_route;
  /* Initialize mems driver interface */
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.mdelay = platform_delay;
  dev_ctx.handle = &SENSOR_BUS;

  /* Init test platform */
  platform_init();

  /* Wait sensor boot time */
  platform_delay(BOOT_TIME);

  /* Check device ID */
  iis3dwb_device_id_get(&dev_ctx, &whoamI);

  if (whoamI != IIS3DWB_ID)
    while (1);

  /* Restore default configuration */
  iis3dwb_reset_set(&dev_ctx, PROPERTY_ENABLE);

  do {
    iis3dwb_reset_get(&dev_ctx, &rst);
  } while (rst);

  /* Enable Block Data Update */
  iis3dwb_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

  /* Set full scale */
  iis3dwb_xl_full_scale_set(&dev_ctx, IIS3DWB_8g);

  /*
   * Set FIFO watermark (number of unread sensor data TAG + 6 bytes
   * stored in FIFO) to FIFO_WATERMARK samples
   */
  iis3dwb_fifo_watermark_set(&dev_ctx, FIFO_WATERMARK);

  /* Set FIFO batch XL ODR to 12.5Hz */
  iis3dwb_fifo_xl_batch_set(&dev_ctx, IIS3DWB_XL_BATCHED_AT_26k7Hz);

  /* Set FIFO mode to Stream mode (aka Continuous Mode) */
  iis3dwb_fifo_mode_set(&dev_ctx, IIS3DWB_STREAM_MODE);

  /* Set Output Data Rate */
  iis3dwb_xl_data_rate_set(&dev_ctx, IIS3DWB_XL_ODR_26k7Hz);
  iis3dwb_fifo_timestamp_batch_set(&dev_ctx, IIS3DWB_DEC_8);
  iis3dwb_timestamp_set(&dev_ctx, PROPERTY_ENABLE);

  /* Wait samples */
  while (1) {
    uint16_t num = 0, k;

    /* Read watermark flag */
    iis3dwb_fifo_status_get(&dev_ctx, &fifo_status);

    if (fifo_status.fifo_th == 1) {
      num = fifo_status.fifo_level;
      snprintf((char *)tx_buffer, sizeof(tx_buffer), "-- FIFO num %d \r\n", num);
      tx_com(tx_buffer, strlen((char const *)tx_buffer));

      /* read out all FIFO entries in a single read */
      iis3dwb_fifo_out_multi_raw_get(&dev_ctx, fifo_data, num);

      for (k = 0; k < num; k++) {
        iis3dwb_fifo_out_raw_t *f_data;

        /* print out first two and last two FIFO entries only */
        if (k > 1 && k < num - 2)
          continue;

        f_data = &fifo_data[k];

        /* Read FIFO sensor value */
        datax = (int16_t *)&f_data->data[0];
        datay = (int16_t *)&f_data->data[2];
        dataz = (int16_t *)&f_data->data[4];
        ts = (int32_t *)&f_data->data[0];

        switch (f_data->tag >> 3) {
        case IIS3DWB_XL_TAG:
          snprintf((char *)tx_buffer, sizeof(tx_buffer), "%d: ACC [mg]:\t%4.2f\t%4.2f\t%4.2f\r\n",
                  k,
                  iis3dwb_from_fs8g_to_mg(*datax),
                  iis3dwb_from_fs8g_to_mg(*datay),
                  iis3dwb_from_fs8g_to_mg(*dataz));
          tx_com(tx_buffer, strlen((char const *)tx_buffer));
          break;
        case IIS3DWB_TIMESTAMP_TAG:
          snprintf((char *)tx_buffer, sizeof(tx_buffer), "%d TIMESTAMP [ms] %ld\r\n", k, (long)*ts);
          tx_com(tx_buffer, strlen((char const *)tx_buffer));
          break;
        default:
          break;
        }
      }

      snprintf((char *)tx_buffer, sizeof(tx_buffer), "------ \r\n\r\n");
      tx_com(tx_buffer, strlen((char const *)tx_buffer));
    }
  }
}


static int32_t platform_read(void *h,uint8_t r,uint8_t *d,uint16_t n){return official_read(h,r,d,n);}
static int32_t platform_write(void *h,uint8_t r,const uint8_t *d,uint16_t n){return official_write(h,r,d,n);}
static void tx_com(uint8_t *d,uint16_t n){official_console(d,n);}
static void platform_delay(uint32_t n){official_delay(n);}
static void platform_init(void){official_init();}
