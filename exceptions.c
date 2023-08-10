#include "exceptions.h"
#include <stdio.h>
void printFFErr(FRESULT res){
	switch (res)
	{
	case FR_DISK_ERR:
		printf("A hard error occurred in the low level disk I/O layer!\r\n");
		break;
	case FR_INT_ERR:
		printf("Internal ff error!\r\n");
		break;
	case FR_NOT_READY:
		printf("Disk not read!\r\n");
		break;
	case FR_NO_FILE:
		printf("File not found!\r\n");
		break;
	case FR_NO_PATH:
		printf("Could not find the path!\r\n");
		break;
	case FR_INVALID_NAME:
		printf("The path name format is invalid!\r\n");
		break;
	case FR_DENIED:
		printf("Access denied due to prohibited access or directory full!\r\n");
		break;
	case FR_EXIST:
		printf("Access denied due to prohibited access!\r\n");
		break;
	case FR_INVALID_OBJECT:
		printf("The file/directory object is invalid!\r\n");
		break;
	case FR_WRITE_PROTECTED:
		printf("The physical drive is write protected!\r\n");
		break;
	case FR_INVALID_DRIVE:
		printf("The logical drive number is invalid!\r\n");
		break;	
	case FR_NOT_ENABLED:
		printf("The volume has no work area!\r\n");
		break;
	case FR_NO_FILESYSTEM:
		printf("There is no valid FAT volume!\r\n");
		break;
	case FR_MKFS_ABORTED:
		printf("The f_mkfs() aborted due to any problem!\r\n");
		break;
	case FR_TIMEOUT:
		printf("Could not get a grant to access the volume within defined period!\r\n");
		break;
	case FR_LOCKED:
		printf("The operation is rejected according to the file sharing policy!\r\n");
		break;
	case FR_NOT_ENOUGH_CORE:
		printf("LFN working buffer could not be allocated!\r\n");
		break;
	case FR_TOO_MANY_OPEN_FILES:
		printf("Number of open files > FF_FS_LOCK!\r\n");
		break;
	case FR_INVALID_PARAMETER:
		printf("Given parameter is invalid!\r\n");
		break;
	default:
		break;
	}
	
}