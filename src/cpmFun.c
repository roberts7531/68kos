#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpmFun.h"
#include <string.h>
#include "../fatfs/ff.h"
Disk disks[]={
    {'A',NULL},
    {'B',NULL},
    {'C',NULL}
};
Image* imageList;
void cpmDiskRead(uint32_t lba,uint32_t driveCount,void*dest){

    uint16_t drive = driveCount>>16;
    uint16_t count = driveCount&0xffff;
    UINT bytePos = lba*512;
    UINT finalCount = count *512;
    FIL file;
    FRESULT res = f_open(&file,disks[drive].image->fileName,FA_READ|FA_OPEN_EXISTING);
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
    FRESULT fr;
    DIR dir;
    FILINFO fno;
    imageList = malloc(sizeof(Image));
    if(imageList==NULL) printf("Malloc failed\r\n");
    printf("Select a disk image:\n");
    int j=0;
    fr = f_findfirst(&dir,&fno,"","*.IMG");
    while(fr==FR_OK && fno.fname[0]){
        imageList[j].number =j;
        strcpy(imageList[j].fileName,fno.fname);
        printf("%d:%s\n",j,fno.fname);
        j++;
        imageList = realloc(imageList,sizeof(imageList)+sizeof(imageList[0]));
        fr = f_findnext(&dir,&fno);
    }
 
    f_closedir(&dir);

    for(int i=0;i<sizeof(disks)/sizeof(disks[0]);i++){
        printf("Image for drive %c\n",disks[i].driveLetter);
        int letter;
        scanf("%d",&letter);
            for(int j=0;j<sizeof(*imageList)/sizeof(imageList[0]);j++){
                if(imageList[j].number==letter){
                    disks[i].image=&imageList[j];
                    break;
                }
            }
        if(disks[i].image==NULL){
            printf("Bad number!\r\n");
            break;
        }

    }
    
    

    printf("Init trap! \r\n");
}
void cpmDbg(){
    printf("debug print\r\n");
}