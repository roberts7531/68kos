#include "ccp.h"
#include "../fatfs/ff.h"
#include "exceptions.h"
#include <stdio.h>
#include <string.h>
#include "srecord.h"
#include <stdlib.h>
#include "../base64/b64.h"

const int consoleSize = 100;

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    
    DIR dir ;
    static FILINFO fno;

    FRESULT fres = f_opendir(&dir, path);                       /* Open the directory */
	if(fres){
		printFFErr(fres);
		return fres;
	}
	
    while(1) {
        fres = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (fres != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        printf("%s", fno.fname);
		if (fno.fattrib & AM_DIR){
			printf(" D\n");
		}else{
			printf("\n");
		}   
    }
    f_closedir(&dir);
    
    return fres;
}
void listRoot(){
    char buffer[256];
	memset(buffer,0,256);
    strcpy(buffer, "");
    scan_files(buffer);
}

void listDir(){
    char buffer[256];
    f_getcwd(buffer,256);
	scan_files(buffer);
}
void formatDisk(){
    BYTE work[FF_MAX_SS];
    FRESULT fres = f_mkfs("", 0, work, sizeof work);
	if(fres){
		printFFErr(fres);
		return;
	}
	printf("format done");
}
void makeDir(char* name){
    FRESULT fres = f_mkdir(name);
    if(fres) printFFErr(fres);
}
void changeDir(char* name){
    FRESULT fres = f_chdir(name);
    if(fres) printFFErr(fres);
}
void delete(char* name){
    FRESULT fres = f_unlink(name);
	if(fres){
		printFFErr(fres);
	}
}
void uploadSr(char* name){
    FIL file;
	FRESULT fres = f_open(&file,name,FA_CREATE_ALWAYS|FA_WRITE);
	if(fres){
		printFFErr(fres);
		return;
	}
		
	printf("File created!\r\n");
	uploadSrec(&file);
}
void listFile(char* name){
    FIL file;
	FRESULT fres = f_open(&file,name,FA_READ|FA_OPEN_EXISTING);
	if(fres){
		printFFErr(fres);
		return;
	}
		
	char buffer[200];
	while(1){
		f_gets(buffer,200,&file);
		printf("%s",buffer);
		if(f_eof(&file))break;
	}
}
void runSr(char* name){
    FIL file;
	FRESULT fres = f_open(&file,name,FA_READ|FA_OPEN_EXISTING);
	if(fres){
		printFFErr(fres);
		return;
	}
		
	readSrecord(&file);
}

void receiveFile(char* name){
    FIL file;
    FRESULT fres = f_open(&file,name,FA_CREATE_NEW|FA_WRITE);
    if(fres){
        printFFErr(fres);
        return;
    }
    printf("receive start!\r\n");
    char buffer[255];
    memset(buffer,'\0',255);
    __uint8_t buffEnd = 0;
    char c = getchar();
    while(c!=0){
        
        buffer[buffEnd]=c;
        if(buffEnd<255){
            buffEnd++;
        }else{
            printf("Buffer len exceeded!\r\n");
        }
        if(c=='\n'||c=='\r'){
            UINT bw;
            size_t decodedSize;
            char * decoded = b64_decode_ex(buffer,strlen(buffer),&decodedSize);
            f_write(&file,decoded,decodedSize,&bw);
            //printf("Decoded line: %s size:%d\r\n",decoded,decodedSize);
            putchar('S');
            buffEnd=0;
            memset(buffer,'\0',255);
            free(decoded);
        }else{
            putchar('.');
        }
        
        c = getchar();
    }
    f_close(&file);
}
typedef void func(void);

void enterMonitor(){
        func* startup = (func*)0x720000;
        startup();

}

void showHelp(void);
const static struct {
    const char *name;
    void (*func)(char* arg1,char*arg2,char*arg3);
    const char *description;
    const uint8_t parmCount;
} function_map []= {
    {"DIR",listDir,"Lists Directory contents",0},
    {"FORMAT",formatDisk,"Formats disk to FAT32",0},
    {"MKDIR",makeDir,"Create a directory",1},
    {"CD",changeDir,"Change directory",1},
    {"DEL",delete,"Delete",1},
    {"UPLOAD",uploadSr,"Upload SREC file",1},
    {"LIST",listFile,"List ascii file to screen",1},
    {"RUN",runSr,"Load and execute SREC file",1},
    {"HELP",showHelp,"Show this help!",0},
    {"DOWNLOAD",receiveFile,"Receive a base64 binary",1},
    {"MONITOR",enterMonitor,"Enter zBug monitor",0}
};

void showHelp(){

    for (int i = 0; i < (sizeof(function_map) / sizeof(function_map[0])); i++) {
        printf("%s - %s, takes %d parameters!\r\n",function_map[i].name,function_map[i].description,function_map[i].parmCount);
    } 
}

void processCommand(struct cmdL* cmd){

    for (int i = 0; i < (sizeof(function_map) / sizeof(function_map[0])); i++) {
        if (!strcmp(function_map[i].name, cmd->arg0) && function_map[i].func) {
            if(cmd->len-1!=function_map[i].parmCount){
                printf("Expected %d parameters!\r\n",function_map[i].parmCount);
                return;
            }
            function_map[i].func(cmd->arg1,cmd->arg2,cmd->arg3);
            return ;
        }
    } 
    printf("Function not found!\r\n");
    
    
	

}

void parseConsoleLine(char * console){
	char*token;
	struct cmdL command;
	command.len=1;

	token = strtok(console," ");
	
	if(token==NULL){
		return ;
	}else{
		command.arg0 = malloc(strlen(token)+1);
		if(command.arg0!=NULL){
			strcpy(command.arg0,token);
		}

		token = strtok(NULL," ");
		
		if(token !=NULL){
			command.len++;
			command.arg1 = malloc(strlen(token)+1);
			if(command.arg1!=NULL){
				strcpy(command.arg1,token);
			}

			token = strtok(NULL," ");
			if(token!=NULL){
				command.len++;
				command.arg2 = malloc(strlen(token)+1);
				if(command.arg2!=NULL){
					strcpy(command.arg2,token);
				}
				token = strtok(NULL," ");
				if(token!=NULL){
					command.len++;
					command.arg3 = malloc(strlen(token)+1);
					if(command.arg3!=NULL){
						strcpy(command.arg3,token);
					}
				}
			}
		}

	}
	printf("\r\n");
	processCommand(&command);
	memset(console,0,consoleSize);
	return ;
}