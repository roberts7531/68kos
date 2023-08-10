#include <stdint.h>
#include <stdio.h>
#include "fatfs/ff.h"
void cpmDiskRead(uint32_t lba,uint32_t driveCount,void*dest){

    uint16_t drive = driveCount>>16;
    uint16_t count = driveCount&0xffff;
    UINT bytePos = lba*512;
    UINT finalCount = count *512;
    FIL file;
    FRESULT res = f_open(&file,"A.IMG",FA_READ|FA_OPEN_EXISTING);
    if(res){
        printf("Failed to open A.img!\r\n");
    }
    
    f_lseek(&file,bytePos);
    UINT br;
    

    res = f_read(&file,dest,finalCount,&br);
    f_close(&file);
}
void cpmDiskWrite(uint32_t lba,uint32_t driveCount,void*src){
    uint16_t drive = driveCount>>16;
    uint16_t count = driveCount&0xffff;
    UINT bytePos = lba*512;
    UINT finalCount = count *512;
    FIL file;
    FRESULT res = f_open(&file,"A.IMG",FA_READ|FA_WRITE);
    if(res){
        printf("Failed to open A.img!\r\n");
    }
    f_lseek(&file,bytePos);
    UINT bw;
    res = f_write(&file,src,finalCount,&bw);
    if(res){
        printf("Write fail!\r\n");
    }
    f_close(&file);
}
void cpmInit(){
    printf("Init trap! \r\n");
}
void cpmDbg(){
    printf("debug print\r\n");
}