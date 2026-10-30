/* FatFs metadata/allocation only while FIFO is stopped. Capture writes exclusively
 * to a closed, persisted, contiguous preallocated file, with no FAT access. */
#include "fat_volume.h"
#include "ff.h"
#include "diskio.h"
#include <string.h>
#ifndef R3_FILE_PREFIX
#define R3_FILE_PREFIX "RUN"
#endif
DurableStatus durable_status;
static SdSpi *card;static FATFS fs;static FIL file;static uint8_t work[4096];
static int failure(uint32_t n){durable_status.error=n;return -1;}
DSTATUS disk_initialize(BYTE d){return d||!card||!card->initialized||card->failed?STA_NOINIT:0;}
DSTATUS disk_status(BYTE d){return disk_initialize(d);}
DRESULT disk_read(BYTE d,BYTE *b,LBA_t lba,UINT count){if(d||!card)return RES_NOTRDY;for(UINT i=0;i<count;i++)if(sd_spi_read(card,lba+i,b+512*i))return RES_ERROR;return RES_OK;}
int durable_sector(uint32_t lba,const uint8_t b[512])
{
 SdWriter w={0};if(!card||sd_writer_begin(&w,card,lba,b))return failure(101);
 while(w.active)if(sd_writer_step(&w))break;
 if(w.error||!w.done){durable_status.io_error=w.error;durable_status.io_uncertain=w.uncertain;(void)sd_spi_quiesce(card);return failure(102);}
 durable_status.io_writes++;return 0;
}
DRESULT disk_write(BYTE d,const BYTE *b,LBA_t lba,UINT count){if(d||!card)return RES_NOTRDY;for(UINT i=0;i<count;i++)if(durable_sector(lba+i,b+512*i))return RES_ERROR;return RES_OK;}
DRESULT disk_ioctl(BYTE d,BYTE cmd,void *p){if(d||!card)return RES_NOTRDY;switch(cmd){case CTRL_SYNC:return sd_spi_quiesce(card)?RES_ERROR:RES_OK;case GET_SECTOR_COUNT:*(LBA_t*)p=card->sectors;return RES_OK;case GET_SECTOR_SIZE:*(WORD*)p=512;return RES_OK;case GET_BLOCK_SIZE:*(DWORD*)p=64;return RES_OK;default:return RES_PARERR;}}
int durable_mount(SdSpi *s)
{
 card=s;if(sd_writer_r3_gate(s))return failure(1);/* Original exact CID check. */
 s->begin_lba=0;s->limit_lba=s->sectors;
 FRESULT r=f_mount(&fs,"0:",1);durable_status.mounted=r==FR_OK;durable_status.fs_type=fs.fs_type;durable_status.cluster_sectors=fs.csize;
 if(r==FR_NO_FILESYSTEM)return 1;
 return r?failure(2):0;
}
int durable_format(void)
{
 if(!card||card->failed||card->sectors!=62333952u||!card->write_enabled)return failure(3);
 MKFS_PARM m={.fmt=FM_FAT32,.n_fat=2,.align=2048,.n_root=0,.au_size=32768};
 if(f_mkfs("0:",&m,work,sizeof work)!=FR_OK||f_mount(&fs,"0:",1)!=FR_OK||fs.fs_type!=FS_FAT32)return failure(4);
 durable_status.mounted=1;durable_status.formatted=1;durable_status.fs_type=fs.fs_type;durable_status.cluster_sectors=fs.csize;
 static const char info[]="MSM TR2 R3-DURABLE experimental FAT32 test card\r\nCID SPI: 0353445344333247858bf0923c01927b\r\nOnly TR2-002; NOT PRODUCTION. No UTC clock asserted.\r\nFiles RUN0003.TR2, RUN0004.TR2, RUN0005.TR2 are preallocated only when their RAM trigger is submitted.\r\nUse the versioned Python importer, not file size, to determine completion and valid prefix.\r\n";
 UINT n=0;if(f_open(&file,"0:/TR2INFO.TXT",FA_WRITE|FA_CREATE_NEW)!=FR_OK||f_write(&file,info,sizeof info-1,&n)!=FR_OK||n!=sizeof info-1||f_sync(&file)!=FR_OK||f_close(&file)!=FR_OK)return failure(5);
 return sd_spi_quiesce(card)?failure(6):0;
}
int durable_prepare(uint32_t run,uint32_t bytes,uint64_t session,const uint8_t header[512])
{
 if(!durable_status.mounted||fs.fs_type!=FS_FAT32||run<3||run>5||bytes%512||bytes<2048)return failure(7);
 DWORD free_clusters;FATFS *available;
 if(f_getfree("0:",&free_clusters,&available)!=FR_OK||available!=&fs)return failure(29);
 durable_status.free_clusters=free_clusters;durable_status.required_clusters=(bytes/512+fs.csize-1u)/fs.csize;
 if(free_clusters<durable_status.required_clusters)return failure(30);
 char name[]="0:/" R3_FILE_PREFIX "0000.TR2";name[9]=(char)('0'+run);
 if(f_open(&file,name,FA_WRITE|FA_CREATE_NEW)!=FR_OK)return failure(8);
 if(f_expand(&file,bytes,1)!=FR_OK||f_sync(&file)!=FR_OK)return failure(9);
 uint32_t first=file.obj.sclust;uint32_t lba=(uint32_t)fs.database+(first-2u)*fs.csize;
 /* Validate every allocated cluster through FatFs rather than assuming extent. */
 for(uint32_t off=0;off<bytes;off+=(uint32_t)fs.csize*512u){if(f_lseek(&file,off+1)!=FR_OK||file.clust!=first+off/(fs.csize*512u))return failure(10);}
 if(f_close(&file)!=FR_OK||lba>=card->sectors||bytes/512>card->sectors-lba)return failure(11);
 durable_status.first_lba=lba;durable_status.sectors=bytes/512;durable_status.run=run;durable_status.session=session;
 /* Publish a CRC header and invalidate footer before acquisition. No FAT writes
  * remain in the capture window. Garbage in untouched sectors is not trusted. */
 memset(work,0,512);if(durable_sector(lba+bytes/512-1,work)||durable_sector(lba,header))return failure(12);
 return sd_spi_quiesce(card)?failure(13):0;
}
/* A fresh file read through FatFs, only with FIFO stopped; catches corruption
 * before longer gates. Independent Python/PC extraction remains mandatory. */
#include "archive_format.h"
#include "stream_hash.h"
static uint32_t get32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int durable_verify_last(const uint8_t expected[32])
{
 char name[]="0:/" R3_FILE_PREFIX "0000.TR2";name[9]=(char)('0'+durable_status.run);UINT n;
 uint8_t footer[512];
 if(!durable_status.run||f_open(&file,name,FA_READ)!=FR_OK)return failure(20);
 if(f_size(&file)!=durable_status.sectors*512u||f_lseek(&file,f_size(&file)-512)!=FR_OK||f_read(&file,footer,512,&n)!=FR_OK||n!=512)return failure(21);
 if(memcmp(footer,"TR2END01",8)||(get32(footer+8)!=1&&get32(footer+8)!=2&&get32(footer+8)!=3)||get32(footer+12)!=1||get32(footer+24)!=durable_status.run||get32(footer+508)!=durable_crc(footer,508)||memcmp(expected,footer+96,32))return failure(22);
 uint8_t id[8];durable_put64(id,durable_status.session);if(memcmp(id,footer+16,8))return failure(23);
 uint32_t records=get32(footer+36),samples=0;R3Hash h;r3_hash_init(&h);
 if(records>durable_status.sectors-2||f_lseek(&file,512)!=FR_OK)return failure(24);
 for(uint32_t seq=0;seq<records;seq++){
  if(f_read(&file,work,512,&n)!=FR_OK||n!=512||!durable_record_valid(work,durable_status.session,seq,samples))return failure(25);
  unsigned count=work[36];if(seq+1<records&&count!=78)return failure(26);r3_hash_update(&h,work+44,count*6);samples+=count;
 }
 uint8_t actual[32];r3_hash_finish(&h,actual);
 if(samples!=get32(footer+28)||samples*6!=get32(footer+40)||memcmp(actual,expected,32)||f_close(&file)!=FR_OK)return failure(27);
 return sd_spi_quiesce(card)?failure(28):0;
}
