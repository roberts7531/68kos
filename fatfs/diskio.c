/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include <stdio.h>
/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
/*volatile  uint16_t *const IDE_DATA = (volatile uint16_t *const) 0x900000;
volatile  char *const IDE_ERROR = (volatile char *const) 0x900003;
volatile  char *const IDE_SECCOUNT = (volatile char *const) 0x900005;
volatile  char *const IDE_LBA0 = (volatile char *const) 0x900007;
volatile  char *const IDE_LBA1 = (volatile char *const) 0x900009;
volatile  char *const IDE_LBA2 = (volatile char *const) 0x90000b;
volatile  char *const IDE_LBA3 = (volatile char *const) 0x90000d;
volatile  char *const IDE_STATUS = (volatile char *const) 0x90000f;
*/
struct Ide {
	uint16_t	data;
	uint8_t		filler0;
	uint8_t		error;
	uint8_t 	filler1;
	uint8_t 	seccount;
	uint8_t 	filler2;
	uint8_t 	lba0;
	uint8_t 	filler3;
	uint8_t 	lba1;
	uint8_t 	filler4;
	uint8_t 	lba2;
	uint8_t 	filler5;
	uint8_t 	lba3;
	uint8_t 	filler6;
	uint8_t 	status;
};
struct Ide8 {
	uint8_t		filler;
	uint8_t		data;
	uint8_t		filler0;
	uint8_t		error;
	uint8_t 	filler1;
	uint8_t 	seccount;
	uint8_t 	filler2;
	uint8_t 	lba0;
	uint8_t 	filler3;
	uint8_t 	lba1;
	uint8_t 	filler4;
	uint8_t 	lba2;
	uint8_t 	filler5;
	uint8_t 	lba3;
	uint8_t 	filler6;
	uint8_t 	status;
};
volatile struct Ide* disk = (struct Ide*)NULL; 
volatile struct Ide8* disk8 = (struct Ide8*)NULL; 
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
int waitBusyTimeoutFast(){
	for(uint32_t i = 0;i<0xfff;i++){
		if(!(disk->status&0x80)){
			if(disk->status&0x40){
				return 0;
			}
		}	
	}
	return 1;
}
int isIdePresent(uint32_t address){
	if(disk==NULL){
		disk = (struct Ide*)address;
		disk8 = (struct Ide8*)address;
		if(!waitBusyTimeoutFast()){
			return 1;
		}
	}
	disk = (struct Ide*)NULL;
	disk8 = (struct Ide8*)NULL;
	return 0;


}
int waitBusyTimeout(){
	for(uint32_t i = 0;i<0xfffff;i++){
		if(!(disk->status&0x80)){
			if(disk->status&0x40){
				return 0;
			}
		}	
	}
	printf("Drive not ready! error:%x\r\n",disk->error);
	return 1;
}
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{

	
	return RES_OK;
}




int waitDRQTimeout(){
	for(uint32_t i = 0;i<0xfffff;i++){
		if(!(disk->status&0x80)){
			if(disk->status&0x08){
				return 0;
			}
		}
		
	}
	printf("DRQ not ready!\r\n");
	return 1;
}
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
int is8Bit;
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	disk->lba0 = 0x55;
	if(disk->lba0!=0x55){
		printf("No disk!\r\n");
		return RES_NOTRDY;
	}
	if(waitBusyTimeout())	return RES_NOTRDY;
	
	
	int run = 1;
	while(run){
		printf("use 8bit or 16bit IDE interface? (8/6)\r\n");
		char c = getchar();
		switch (c)
		{
		case '8':
			is8Bit = 1;
			run = 0;
			break;
		case '6':
			is8Bit = 0;
			run = 0;
			break;
		default:
			break;
		}
	}
	if(is8Bit){
		disk8->error = 0x1;
		disk8->status= 0xef;
	}
	return RES_OK;
}

int setLBA(LBA_t sector){
	//if(waitBusyTimeout())return RES_NOTRDY;
	disk->lba0 = sector &0xff;
	//if(waitBusyTimeout())return RES_NOTRDY;
	disk->lba1 = (sector>>8) &0xff;
	//if(waitBusyTimeout())return RES_NOTRDY;
	disk->lba2 = (sector>>16) &0xff;
	//if(waitBusyTimeout())return RES_NOTRDY;
	disk->lba3 = ((sector>>24) &0xef)|0xe0;
	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	if(waitBusyTimeout())return RES_NOTRDY;
	disk->seccount = count;
	if(setLBA(sector)) return RES_NOTRDY;
	if(waitBusyTimeout())return RES_NOTRDY;
	disk->status = 0x20;
	if(waitBusyTimeout())return RES_NOTRDY;
	if(waitDRQTimeout()) return RES_NOTRDY;
	for(int i=0;i<256*count;i++){
		//if(waitDRQTimeout()) return RES_NOTRDY;
		if(is8Bit){
			*buff = disk8->data;
			buff++;
			//if(waitDRQTimeout()) return RES_NOTRDY;
			*buff = disk8->data;
			buff++;
		}else{
			uint16_t data = disk->data;
			*buff = data &0xff;
			buff++;
			*buff = (data>>8) & 0xff;
			buff++;
		}	
	}

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if(waitBusyTimeout())return RES_NOTRDY;
	disk->seccount = count;
	if(setLBA(sector)) return RES_NOTRDY;
	if(waitBusyTimeout())return RES_NOTRDY;
	disk->status = 0x30;
	if(waitBusyTimeout())return RES_NOTRDY;
	if(waitDRQTimeout()) return RES_NOTRDY;
	for(int i=0;i<256*count;i++){
		if(waitDRQTimeout()) return RES_NOTRDY;
		if(is8Bit){
			disk8->data = *buff;
			buff++;
			disk8->data = *buff;
			buff++;
		}else{
			uint16_t data = *buff;
			buff++;		
			data |= (*buff<<8);
			buff++;
			disk->data = data;
		}  	
	}
	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	printf("ioctl command:%x\r\n",cmd);
	if(cmd == GET_SECTOR_COUNT){
	LBA_t* seccount = (LBA_t*)buff;
	*seccount = 0xFFFFFFF;
	//*seccount = 0xFFF;
	}else if(cmd == GET_BLOCK_SIZE){
	DWORD* bs = (DWORD*)buff;
	*bs = 1;
	}
	



	return RES_OK;
}

