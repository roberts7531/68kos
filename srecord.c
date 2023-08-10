#include <stdio.h>
#include "main.h"
#include <stdint.h>
#include <stdlib.h>
#include "fatfs/ff.h"
FIL*file;
char inCharD(){
	char ret;
	UINT br;
	fr = f_read(file,&ret,1,&br);
	if(fr) {printf("Read fail! %x\r\n",fr);
		return 0;}
	return ret;
}
int running ;
void seekToStart(){
    char c =  inChar();
    while(c!='S'){
        c = inChar();
    };
    
}
void seekToStartD(){
    char c =  inCharD();
    while(c!='S'){
        c = inCharD();
    };
    
}
uint8_t nextByte(){
	char bytes[3] = "  ";
	bytes[0]=inCharD();
	bytes[1]=inCharD();
    bytes[2] = '\0';
	return (uint8_t) strtol(bytes,NULL,16);
}
int runningRecCount;
int sumLastAddr;
uint32_t readAddress(uint8_t* bytes, int len){
	sumLastAddr = 0;
	uint32_t address = 0;
	uint8_t hi,lo,addrHi,addrMid,addrLo,addrMidHi,addrMidLo;

	switch(len){
		case 16:
			hi = bytes[0];
			lo = bytes[1];
			sumLastAddr = hi+lo;
			address = hi;
			address = (address<<8)|lo;
			return address;
		case 24:
			addrHi = bytes[0];
			addrMid = bytes[1];
			addrLo = bytes[2];
			address = addrHi;
			address = (address<<8)|addrMid;
			address = (address<<8)|addrLo;
			sumLastAddr = addrHi+addrMid+addrLo;
			return address;
		case 32:
			addrHi = bytes[0];
			addrMidHi = bytes[1];
			addrMidLo = bytes[2];
			addrLo = bytes[3];
			address = addrHi;
			address = (address<<8)|addrMidHi;
			address = (address<<8)|addrMidLo;
			address = (address<<8)|addrLo;
			sumLastAddr = addrHi+addrMidHi+addrMidLo+addrLo;
			return address;
	}
	return 0;
}
void readHeader(uint8_t* bytes,int length){
    uint8_t sum = 0;
	
	sum = length;
	for(int i =2;i<length-1;i++){
		uint8_t byte = bytes[i];
        putchar(byte);
		sum+=byte;
	}
	putchar('\n');
	sum = 0xff-sum;
	if(bytes[length-1]!=sum){
		printf("\r\nChecksum failed!\r\n");
        running=0;
	}
}

void readData(uint8_t* bytes,int length,int addrLen){
    //printf("Data\r\n");
				uint8_t* address =(uint8_t*)readAddress(bytes,addrLen);
				uint8_t sum = length+sumLastAddr;
				for(int i =addrLen/8;i<length-1;i++){
					uint8_t byte = bytes[i];
                    *address = byte;
                    address++;
					sum+=byte;

				}
				//printf("Address: %lx",address);
				uint8_t checksum = bytes[length-1];
				sum = 0xff-sum;
				if(checksum==sum){
					//printf("\r\nChecksum passed!\r\n");
                    runningRecCount++;
				}else{
					printf("\r\nChecksum failed!\r\n");
                            running=0;

				}
}
int targetCount;
void verifyCount(uint8_t* bytes,int length){
    int count = readAddress(bytes,16);
    int checksum = bytes[length-1];
    int sum = 0xff-(sumLastAddr+length);
    targetCount = count;
    if(sum!=checksum){
        printf("Checksum fail");
                running=0;

    }
        
}
typedef void func(void);

void readTermination8(uint8_t* bytes, int length){
    func* startup = (func*)readAddress(bytes,24);
	printf("StartAddr: %p\r\n",startup);
    int sum = 0xff-(length+sumLastAddr);
    if(bytes[length-1]==sum){
        if(targetCount ==runningRecCount||targetCount==0){
            printf("Count passed\r\n");
            startup();
        }else{
            printf("Expected %d, but got %d records!\r\n",targetCount,runningRecCount);
        }
	}else{
		printf("\r\nChecksum failed!\r\n");
                running=0;

	}
}
void readTermination9(uint8_t* bytes, int length){
    func* startup = (func*)readAddress(bytes,16);
	printf("StartAddr: %p\r\n",startup);
    int sum = 0xff-(length+sumLastAddr);
    if(bytes[length-1]==sum){
        if(targetCount ==runningRecCount||targetCount==0){
            printf("Count passed\r\n");
			f_close(file);
            startup();
        }else{
            printf("Expected %d, but got %d records!\r\n",targetCount,runningRecCount);
        }
	}else{
		printf("\r\nChecksum failed!\r\n");
                running=0;

	}
}
void uploadSrec(FIL*file){
	int isLast=0;
	while(1){
		putchar('S');
		seekToStart();
		char asciiType = inChar();
		
		f_putc('S',file);
		f_putc(asciiType,file);
		if(asciiType=='8') isLast=1;
		char c;
		do{
			c = inChar();
			f_putc(c,file);
		}while(c!='\n');
		if(!isLast){
			continue;
		}else{
			f_close(file);
			break;
		}
	}
}/*
uint32_t getEntryPoint(FIL*src){
	uint8_t bytes[10];
	while(1){
		file=src;
		seekToStartD();
		char asciiType = inCharD();
    	char recordType = asciiType-'0';
		if(recordType!=8) continue;
		nextByte();
		bytes[0]=nextByte();
		bytes[1]=nextByte();
		bytes[2]=nextByte();
		return readAddress(&bytes,24);

	}
}/*
void convToBin(FIL*src,FIL*dest){
	char header[] = "JEF";
	UINT bw;
	fr = f_write(dest,&header,3,&bw);
	if(fr || bw<3){
		printf("Error writing header\r\n");
		return;
	}
	uint32_t address = getEntryPoint(src);
	f_rewind(src);
	fr = f_write(dest,&address,4,&bw);
	if(fr || bw<4){
		printf("Error writing entry point\r\n");
		return;
	}



}*/

void readSrecord(FIL*fileI){
	file=fileI;
    uint8_t bytes[80];
    running = 1;
    while(running){
        
		seekToStartD();
		char asciiType = inCharD();

        char recordType = asciiType-'0';
        int length = nextByte();
        for(int i=0;i<length;i++){
            bytes[i]=nextByte();
        }
		

		switch(recordType){
			case 0:
				readHeader(bytes,length);
                runningRecCount = 0;
                targetCount=0;
				break;
            case 1:
                readData(bytes,length,16);
                break;
			case 2:
				readData(bytes,length,24);
				break;
            case 3:
				readData(bytes,length,32);
				break;
			case 8:
            	readTermination8(bytes,length);
                break;
			case 9:
				readTermination9(bytes,length);
				break;
			case 5:
                verifyCount(bytes,length);
				break;
			default:
				printf("Unknown record\r\n");

		}
    }

}