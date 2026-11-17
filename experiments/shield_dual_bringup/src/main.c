/* NON-PRODUCTION shield bench. WAIT at boot; independent commands, no formatter,
 * no Modbus TX, no commissioning or persistent-identity service. */
#include "platform.h"
#include "gates.h"
#include "sd_spi.h"
#include "sd_writer.h"
#include "stm32_fram_storage.h"
#include "ff.h"
#include "diskio.h"
#include <string.h>

typedef struct {
 uint32_t version,stage,command,sequence,error,done_mask,uid[3];
 uint32_t fram_crc,fram_mismatches,fram_restore_mismatches;
 uint32_t sd_crc,sd_bytes,sd_file_verified,sd_file_deleted,sd_writes;
 uint32_t sensor_count;uint8_t fram_id[4],who,ctrl_before[2],ctrl_after[2];
 int16_t xyz[16][3];uint32_t sensor_restore_error;
} Probe;
volatile Probe probe;
volatile uint32_t probe_command,probe_arm,probe_export_confirmed;
uint8_t probe_fram_snapshot[FRAM_BYTES],probe_sd_return[4096],probe_sd_expected[4096];
uint8_t probe_sd_sector0[512];
char probe_sd_path[16]="0:/T1700201.TMP";
SdSpi probe_card;SdWriter probe_writer;
uint32_t probe_sd_uncertain;
static stmdev_ctx_t ctx;
static SPI_HandleTypeDef fram_spi;
static Stm32FramStorage fram;
static FATFS fs;static FIL file;static bool disk_writes;
static uint64_t elapsed;static uint32_t last_cycles;
extern int r3_sd_prepare(void);extern int r3_sd_init(SdSpi*);extern void r3_sd_deselect(void);
uint64_t r3_now(void *unused){(void)unused;uint32_t c=DWT->CYCCNT;elapsed+=(uint32_t)(c-last_cycles);last_cycles=c;return elapsed/160u;}
void SysTick_Handler(void){HAL_IncTick();}
void Error_Handler(void){probe.stage=98;for(;;)__WFI();}
static uint32_t crc32(const uint8_t*b,uint32_t n){uint32_t c=~0u;for(uint32_t i=0;i<n;i++){c^=b[i];for(unsigned j=0;j<8;j++)c=(c>>1)^((c&1)?0xedb88320u:0);}return ~c;}
static void safe_cs(void)
{
 GPIO_InitTypeDef g={0};__HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOD_CLK_ENABLE();__HAL_RCC_GPIOF_CLK_ENABLE();__HAL_RCC_GPIOG_CLK_ENABLE();
 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOF,GPIO_PIN_12,GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOG,GPIO_PIN_4,GPIO_PIN_RESET);
 g.Mode=GPIO_MODE_OUTPUT_PP;g.Pull=GPIO_PULLUP;g.Speed=GPIO_SPEED_FREQ_LOW;g.Pin=GPIO_PIN_9;HAL_GPIO_Init(GPIOC,&g);g.Pin=GPIO_PIN_14;HAL_GPIO_Init(GPIOD,&g);g.Pin=GPIO_PIN_12;HAL_GPIO_Init(GPIOF,&g);g.Pin=GPIO_PIN_4;g.Pull=GPIO_PULLDOWN;HAL_GPIO_Init(GPIOG,&g);
}
static int fram_setup(void)
{
 GPIO_InitTypeDef g={0};RCC_PeriphCLKInitTypeDef p={0};p.PeriphClockSelection=RCC_PERIPHCLK_SPI1;p.Spi1ClockSelection=RCC_SPI1CLKSOURCE_HSI;
 if(HAL_RCCEx_PeriphCLKConfig(&p)!=HAL_OK)return 101;
 __HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_SPI1_CLK_ENABLE();g.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;g.Mode=GPIO_MODE_AF_PP;g.Pull=GPIO_NOPULL;g.Speed=GPIO_SPEED_FREQ_HIGH;g.Alternate=GPIO_AF5_SPI1;HAL_GPIO_Init(GPIOA,&g);
 fram_spi.Instance=SPI1;fram_spi.Init.Mode=SPI_MODE_MASTER;fram_spi.Init.Direction=SPI_DIRECTION_2LINES;fram_spi.Init.DataSize=SPI_DATASIZE_8BIT;
 fram_spi.Init.CLKPolarity=SPI_POLARITY_LOW;fram_spi.Init.CLKPhase=SPI_PHASE_1EDGE;fram_spi.Init.NSS=SPI_NSS_SOFT;fram_spi.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_16;fram_spi.Init.FirstBit=SPI_FIRSTBIT_MSB;
 fram_spi.Init.TIMode=SPI_TIMODE_DISABLE;fram_spi.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;fram_spi.Init.CRCPolynomial=7;fram_spi.Init.NSSPMode=SPI_NSS_PULSE_DISABLE;fram_spi.Init.NSSPolarity=SPI_NSS_POLARITY_LOW;fram_spi.Init.FifoThreshold=SPI_FIFO_THRESHOLD_01DATA;fram_spi.Init.MasterSSIdleness=SPI_MASTER_SS_IDLENESS_00CYCLE;fram_spi.Init.MasterInterDataIdleness=SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;fram_spi.Init.MasterReceiverAutoSusp=SPI_MASTER_RX_AUTOSUSP_DISABLE;fram_spi.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;fram_spi.Init.IOSwap=SPI_IO_SWAP_DISABLE;fram_spi.Init.ReadyMasterManagement=SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;fram_spi.Init.ReadyPolarity=SPI_RDY_POLARITY_HIGH;
 if(HAL_SPI_Init(&fram_spi)!=HAL_OK)return 102;
 return stm32_fram_storage_init(&fram,&fram_spi,GPIOD,GPIO_PIN_14,100)==TR2_OK?0:103;
}
static int fram_readonly(void)
{
 static const uint8_t id[4]={4,0x7f,0x48,3};uint8_t tx[5]={0x9f,255,255,255,255},rx[5]={0},check[256];
 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);HAL_StatusTypeDef s=HAL_SPI_TransmitReceive(&fram_spi,tx,rx,5,100);HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);memcpy((void*)probe.fram_id,rx+1,4);
 if(s!=HAL_OK||memcmp(rx+1,id,4))return 110;
 if(stm32_fram_storage_read(&fram,0,probe_fram_snapshot,FRAM_BYTES)!=TR2_OK)return 111;
 probe.fram_crc=crc32(probe_fram_snapshot,FRAM_BYTES);probe.fram_mismatches=0;
 for(uint32_t off=0;off<FRAM_BYTES;off+=sizeof check){if(stm32_fram_storage_read(&fram,off,check,sizeof check)!=TR2_OK)return 112;for(unsigned i=0;i<sizeof check;i++)probe.fram_mismatches+=check[i]!=probe_fram_snapshot[off+i];}
 return probe.fram_mismatches?113:0;
}
static int fram_restore_test(uint32_t arm)
{
 bool ready=(probe.done_mask&(1u<<1))!=0;uint8_t old[64],pattern[64],read[256];int error=0;
 static const uint8_t expected_id[4]={4,0x7f,0x48,3};
 bool identified=memcmp((const void*)probe.fram_id,expected_id,4)==0;
 if(!bench_fram_write(arm,probe_export_confirmed&&ready,identified,SCRATCH_OFFSET,64))return 120;
 memcpy(old,probe_fram_snapshot+SCRATCH_OFFSET,64);
 if(stm32_fram_storage_read(&fram,SCRATCH_OFFSET,read,64)!=TR2_OK||memcmp(read,old,64))return 121;
 for(unsigned i=0;i<64;i++)pattern[i]=(uint8_t)(old[i]^0xa5u^(i*13u));
 if(stm32_fram_storage_write(&fram,SCRATCH_OFFSET,pattern,64)!=TR2_OK)error=122;
 if(!error&&(stm32_fram_storage_read(&fram,SCRATCH_OFFSET,read,64)!=TR2_OK||memcmp(read,pattern,64)))error=123;
 /* Restore is mandatory even if the pattern/readback failed. It is not a retry. */
 if(stm32_fram_storage_write(&fram,SCRATCH_OFFSET,old,64)!=TR2_OK)return 124;
 probe.fram_restore_mismatches=0;
 for(uint32_t off=0;off<FRAM_BYTES;off+=sizeof read){if(stm32_fram_storage_read(&fram,off,read,sizeof read)!=TR2_OK)return 125;for(unsigned i=0;i<sizeof read;i++)probe.fram_restore_mismatches+=read[i]!=probe_fram_snapshot[off+i];}
 return probe.fram_restore_mismatches?126:error;
}
DSTATUS disk_initialize(BYTE d){return d||!probe_card.initialized||probe_card.failed?STA_NOINIT:0;}
DSTATUS disk_status(BYTE d){return disk_initialize(d);}
DRESULT disk_read(BYTE d,BYTE*b,LBA_t lba,UINT n){if(d||!probe_card.initialized)return RES_NOTRDY;for(UINT i=0;i<n;i++)if(sd_spi_read(&probe_card,lba+i,b+512*i))return RES_ERROR;return RES_OK;}
DRESULT disk_write(BYTE d,const BYTE*b,LBA_t lba,UINT n)
{
 if(d||!disk_writes||!bench_card(probe_card.cid,probe_card.sectors))return RES_WRPRT;
 for(UINT i=0;i<n;i++){
  if(sd_writer_begin(&probe_writer,&probe_card,lba+i,b+512*i))return RES_ERROR;
  while(probe_writer.active){(void)r3_now(0);if(sd_writer_step(&probe_writer))break;}
  if(probe_writer.error||!probe_writer.done){probe_sd_uncertain=probe_writer.uncertain;(void)sd_spi_quiesce(&probe_card);return RES_ERROR;}
  probe.sd_writes++;
 }
 return RES_OK;
}
DRESULT disk_ioctl(BYTE d,BYTE cmd,void*p)
{
 if(d||!probe_card.initialized)return RES_NOTRDY;
 switch(cmd){case CTRL_SYNC:return sd_spi_quiesce(&probe_card)?RES_ERROR:RES_OK;case GET_SECTOR_COUNT:*(LBA_t*)p=probe_card.sectors;return RES_OK;case GET_SECTOR_SIZE:*(WORD*)p=512;return RES_OK;case GET_BLOCK_SIZE:*(DWORD*)p=64;return RES_OK;default:return RES_PARERR;}
}
static int sd_readonly(void)
{
 if(probe_card.failed||probe_sd_uncertain)return 201;
 if(r3_sd_init(&probe_card))return 202;
 if(!bench_card(probe_card.cid,probe_card.sectors))return 203;
 if(sd_spi_read(&probe_card,0,probe_sd_sector0))return 204;
 if(f_mount(&fs,"0:",1)!=FR_OK||fs.fs_type!=FS_FAT32)return 205;
 return probe_card.writes||probe.sd_writes?206:0;
}
static int sd_scratch(uint32_t arm)
{
 UINT n=0;uint8_t tail;int r;FILINFO info;
 if(arm!=WRITE_ARM||!(probe.done_mask&(1u<<2))||!bench_card(probe_card.cid,probe_card.sectors)||probe_card.failed||probe_sd_uncertain)return 210;
 /* A fixed short filename is CREATE_NEW only. No arbitrary path or overwrite. */
 if(strcmp(probe_sd_path,"0:/T1700201.TMP")&&strcmp(probe_sd_path,"0:/T1700101.TMP"))return 211;
 if(f_stat(probe_sd_path,&info)!=FR_NO_FILE)return 212;
 if(sd_writer_r3_gate(&probe_card))return 213;
 probe_card.begin_lba=0;probe_card.limit_lba=probe_card.sectors;disk_writes=true;
 for(unsigned i=0;i<sizeof probe_sd_expected;i++)probe_sd_expected[i]=(uint8_t)((i*37u)^(i>>3)^0xa5u);
 r=f_open(&file,probe_sd_path,FA_WRITE|FA_CREATE_NEW);if(r)return 220+r;
 r=f_write(&file,probe_sd_expected,sizeof probe_sd_expected,&n);if(r||n!=sizeof probe_sd_expected)return 240+r;
 if(f_sync(&file)!=FR_OK||f_close(&file)!=FR_OK)return 260;
 disk_writes=false;
 if(f_open(&file,probe_sd_path,FA_READ)!=FR_OK)return 261;
 if(f_read(&file,probe_sd_return,sizeof probe_sd_return,&n)!=FR_OK||n!=sizeof probe_sd_return)return 262;
 if(f_read(&file,&tail,1,&n)!=FR_OK||n||f_close(&file)!=FR_OK)return 263;
 probe.sd_crc=crc32(probe_sd_return,sizeof probe_sd_return);probe.sd_bytes=sizeof probe_sd_return;
 if(memcmp(probe_sd_return,probe_sd_expected,sizeof probe_sd_return))return 264;
 probe.sd_file_verified=1;return 0;
}
static int sd_remove(uint32_t arm)
{
 FILINFO info;if(arm!=WRITE_ARM||!probe.sd_file_verified||!bench_card(probe_card.cid,probe_card.sectors)||probe_card.failed||probe_sd_uncertain)return 270;
 if(sd_writer_r3_gate(&probe_card))return 273;
 probe_card.begin_lba=0;probe_card.limit_lba=probe_card.sectors;
 disk_writes=true;FRESULT r=f_unlink(probe_sd_path);disk_writes=false;
 if(r||f_stat(probe_sd_path,&info)!=FR_NO_FILE)return 271;
 probe.sd_file_deleted=1;return sd_spi_quiesce(&probe_card)?272:0;
}
static int sensor_who(void){uint8_t who=0;int r=iis3dwb_device_id_get(&ctx,&who);probe.who=who;return r||who!=0x7b?301:0;}
static int sensor_xyz(void)
{
 if(!(probe.done_mask&(1u<<3)))return 310;
 uint8_t saved[2],after[2];int r=0;uint32_t begin=HAL_GetTick();
 if(iis3dwb_read_reg(&ctx,IIS3DWB_CTRL1_XL,&saved[0],1)||iis3dwb_read_reg(&ctx,IIS3DWB_CTRL3_C,&saved[1],1))return 311;
 memcpy((void*)probe.ctrl_before,saved,2);
 if(iis3dwb_block_data_update_set(&ctx,1)||iis3dwb_auto_increment_set(&ctx,1)||iis3dwb_xl_full_scale_set(&ctx,IIS3DWB_8g)||iis3dwb_xl_data_rate_set(&ctx,IIS3DWB_XL_ODR_26k7Hz))r=312;
 probe.sensor_count=0;
 while(!r&&probe.sensor_count<16&&(uint32_t)(HAL_GetTick()-begin)<1000){iis3dwb_status_reg_t st;int16_t v[3];if(iis3dwb_status_reg_get(&ctx,&st)){r=313;break;}if(st.xlda){if(iis3dwb_acceleration_raw_get(&ctx,v)){r=314;break;}memcpy((void*)probe.xyz[probe.sensor_count++],v,sizeof v);}}
 if(!r&&probe.sensor_count!=16)r=315;
 int restore1=iis3dwb_write_reg(&ctx,IIS3DWB_CTRL1_XL,&saved[0],1);
 int restore3=iis3dwb_write_reg(&ctx,IIS3DWB_CTRL3_C,&saved[1],1);
 if(restore1||restore3||iis3dwb_read_reg(&ctx,IIS3DWB_CTRL1_XL,&after[0],1)||iis3dwb_read_reg(&ctx,IIS3DWB_CTRL3_C,&after[1],1))probe.sensor_restore_error=1;
 else{memcpy((void*)probe.ctrl_after,after,2);probe.sensor_restore_error=memcmp(saved,after,2)!=0;}
 return probe.sensor_restore_error?316:r;
}
int main(void)
{
 HAL_Init();probe.version=1;probe.uid[0]=HAL_GetUIDw0();probe.uid[1]=HAL_GetUIDw1();probe.uid[2]=HAL_GetUIDw2();safe_cs();
 uint32_t u[3]={probe.uid[0],probe.uid[1],probe.uid[2]};if(!bringup_uid_allowed(u))Error_Handler();
 BenchClock clock;if(platform_init(&ctx,&clock)||r3_sd_prepare()||fram_setup())Error_Handler();
 probe.stage=1;
 for(;;){
  while(!probe_command){(void)r3_now(0);__WFI();}
  uint32_t cmd=probe_command,arm=probe_arm;probe_command=0;probe_arm=0;probe.command=cmd;probe.sequence++;probe.stage=2;if(cmd<32)probe.done_mask&=~(1u<<cmd);int r=90;
  if(cmd==1)r=fram_readonly();else if(cmd==2)r=sd_readonly();else if(cmd==3)r=sensor_who();else if(cmd==4)r=sensor_xyz();else if(cmd==5)r=fram_restore_test(arm);else if(cmd==6)r=sd_scratch(arm);else if(cmd==7)r=sd_remove(arm);
  disk_writes=false;probe_card.write_enabled=false;r3_sd_deselect();HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
  probe.error=(uint32_t)r;if(!r&&cmd<32)probe.done_mask|=1u<<cmd;probe.stage=r?99:3;
 }
}
