#include <stdlib.h>
#include "../fatfs/ff.h"
#include "../fatfs/diskio.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "srecord.h"
#include <errno.h>
#include <malloc.h>
#include "exceptions.h"
#include "ccp.h"
#include "gpu.h"
FATFS fat;
char buffer[256];
FRESULT fr;
FRESULT res;
volatile  char *const UART_SRA = (volatile char *const) 0x800003;
volatile  char *const UART_TBD = (volatile char *const) 0x800007;

char inChar(){
	while(!(*UART_SRA&0b1));
	char c = *UART_TBD;
	return c;
}
void interruptHandler(uint32_t number){
	switch (number)
	{
	case 2:
		gpuInt();
		break;
	
	default:
		printf("Interrupt %ld\r\n",number);

		break;
	}
}

void __attribute__ ((interrupt)) addrerr(void){
	printf("Address error");
}
void installExceptions(){
	printf("Installing exception handler\r\n");
	uint32_t * location = (uint32_t*) 0xc;
	*location = (uint32_t)addrerr;
}
int isGpuFound=0;
int isIdeFound=0;
void initHw(){
	for(uint32_t i=0x900000;i<=0xf00000;i+= 0x100000){
		printf("Testing %lx:",i);
		volatile uint8_t* pointer = (uint8_t*)i+0x5555;
		*pointer = 0xaa;
		if(*pointer ==0xaa){
			pointer++;
			*pointer = 0x55;
			if(*pointer ==0x55){
				printf("Ram found!\r\n");
				continue;
			}
		}
		

		if(!isGpuFound&&isGpuPresent(i)){
			startGpu();
			printf("Gpu found!\r\n");
			isGpuFound=1;
			continue;
		}else if(!isIdeFound&&isIdePresent(i)){
			printf("Ide found!\r\n");
			isIdeFound=1;
			continue;
		}else{
			printf("Open bus!\r\n");
			
			}
	}
}

int main() {
	initHw();
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
	printf("\r\nCurrent drive contents:\r\n");
	
	fr = f_mount(&fat,"",0);
	if(fr) printFFErr(fr);
	

	listRoot();

	installExceptions();
	
	char* console = (char*)malloc(100);
	memset(console,0,100);
	if(console==NULL){
			malloc_stats();

		printf("Failed to allocate console, bye!\r\n");
				printf("%d\r\n",errno);

		return 1;
	}
	printf(">");
	char* consoleptr = console;
	

	char c;
	while(1){
	c = inChar();
	if(c>96) c-= 32;
	if(c=='\n'||c=='\r'){
		
		printf("\r\n");
		consoleptr = console;
		parseConsoleLine(console);
		f_getcwd(buffer,256);
		printf("%s>",buffer);


	}else{
		if(c==8){
			if(consoleptr-console!=0){
				consoleptr--;
				*consoleptr='\0';
				putchar(c);
			}
		}else{
			if(consoleptr-console < 100){
				putchar(c);
				*consoleptr = c;
				consoleptr++;
			}
		}
	}
	
	/*switch (c)
	{
	case 'l':
		readSrecord();
		break;
	
	case '\n':
		break;
	default:
		printf("Unrecognized cmd\r\n");
		break;
	}*/
	}
	

	
	
	return 0;
}
