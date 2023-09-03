#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "gpu.h"
const uint32_t pattNTable = 0x0000;
const uint32_t coltable  = 0x1e000;
const uint32_t attable = 0x1e000+512;

struct Sprite{
    uint8_t maskC;
    uint8_t mask0[32];
    uint8_t mask1[32];
    uint8_t mask2[32];
    uint8_t attr0[16];
    uint8_t attr1[16];
    uint8_t attr2[16];
};
struct Color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
struct Pallete{
    struct Color colors[16];
};

struct Pallete gamePallete = {
    {
    {7,7,7},
    {0,0,0},
    {7,5,1},
    {2,6,3},
    {2,2,7},
    {3,3,7},
    {6,2,2},
    {2,7,7},
    {7,2,2},
    {7,3,3},
    {6,6,2},
    {7,6,4},
    {1,5,1},
    {6,2,5},
    {6,6,6},
    {7,7,7}}
};

struct Sprite playerSprite = {
    3,
    {0x10,0x0C,0x00,0x04,0x01,0x01,0x03,0x03,
    0x03,0x18,0x38,0x20,0x07,0x0D,0x1C,0x18,
    0x00,0x00,0x80,0x00,0x40,0x40,0xE0,0xC0,
    0x80,0x40,0x60,0x20,0x80,0xC0,0xC0,0xE0},

    {0x00,0x00,0x07,0x1B,0x6F,0x0F,0x3C,0x64,
    0x0C,0x07,0x07,0x27,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0xE0,0x60,0xE0,0x00,0x00,
    0x00,0x80,0x80,0xA0,0x00,0x00,0x00,0x00},

    {0x00,0x00,0x00,0x00,0x6F,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

    {0x04,0x04,0x0B,0x0B,0x0B,0x0B,0x0B,0x0B,
    0x04,0x04,0x04,0x08,0x04,0x04,0x04,0x0B},

    {0x00,0x00,0x44,0x44,0x44,0x44,0x44,0x44,
    0x43,0x43,0x43,0x43,0x00,0x00,0x00,0x00},

    {0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};


const uint8_t brick[]= {
    0x66,0x66,0x66,0x61,
    0x66,0x66,0x66,0x61,
    0x66,0x66,0x66,0x61,
    0x11,0x11,0x11,0x11,
    0x66,0x61,0x66,0x66,
    0x66,0x61,0x66,0x66,
    0x66,0x61,0x66,0x66,
    0x11,0x11,0x11,0x11,
    0x66,0x66,0x66,0x61
};

const uint8_t questionTl[]={
    0x46,0x66,0x66,0x66,
0x6a,0xaa,0xaa,0xaa,
0x6a,0x1a,0xaa,0xaa,
0x6a,0xaa,0xa6,0x66,
0x6a,0xaa,0x66,0x11,
0x6a,0xaa,0x66,0x1a,
0x6a,0xaa,0x66,0x1a,
0x6a,0xaa,0xa1,0x1a
};
const uint8_t questionTr[]={
0x66,0x66,0x66,0x64,
0xaa,0xaa,0xaa,0xa1,
0xaa,0xaa,0xa1,0xa1,
0x66,0xaa,0xaa,0xa1,
0x16,0x6a,0xaa,0xa1,
0xa6,0x61,0xaa,0xa1,
0xa6,0x61,0xaa,0xa1,
0x66,0x61,0xaa,0xa1,
};
const uint8_t questionBl[]={
0x6a,0xaa,0xaa,0xa6,
0x6a,0xaa,0xaa,0xa6,
0x6a,0xaa,0xaa,0xaa,
0x6a,0xaa,0xaa,0xa6,
0x6a,0xaa,0xaa,0xa6,
0x6a,0x1a,0xaa,0xaa,
0x6a,0xaa,0xaa,0xaa,
0x11,0x11,0x11,0x11
};
const uint8_t questionBr[]={
0x61,0x11,0xaa,0xa1,
0x61,0xaa,0xaa,0xa1,
0x11,0xaa,0xaa,0xa1,
0x6a,0xaa,0xaa,0xa1,
0x61,0xaa,0xaa,0xa1,
0x11,0xaa,0xa1,0xa1,
0xaa,0xaa,0xaa,0xa1,
0x11,0x11,0x11,0x11
};
struct Block{
    uint8_t topLeft;
    uint8_t topRight;
    uint8_t bottLeft;
    uint8_t bottRight;
};
struct Block brickB = {
    0,0,0,0
};
struct Block questionB = {
    1,2,3,4
};
struct Block blocks[256];

uint8_t screen[12][32]={0};

volatile uint8_t shadowReg[47];
struct GpuReg{
    volatile uint8_t filler;
    volatile uint8_t rwPort;
    uint8_t filler1;
    volatile uint8_t cmdPort;
    uint8_t filler2;
    volatile uint8_t palPort;
    uint8_t filler3;
    volatile uint8_t dataPort;
};

struct GpuReg* gpu = (struct GpuReg*)0xc00000;

void writeReg(uint8_t reg, uint8_t value){
    shadowReg[reg] = value;
    gpu->cmdPort = value;
    gpu->cmdPort = reg +0x80;
}
uint8_t readReg(uint8_t reg){
    return shadowReg[reg];
}

void writeVram(uint32_t address,uint8_t data){
writeReg(14,(address>>14)&0x07);
gpu->cmdPort = address&0xff;
gpu->cmdPort = ((address>>8)&0x3f)|0b01000000;
gpu->rwPort = data;

}
uint8_t readSReg(uint8_t reg){
    writeReg(15,reg&0x0f);
    return gpu->cmdPort;
}

uint8_t readVram(uint32_t address){
    writeReg(14,(address>>14)&0x07);
    gpu->cmdPort = address&0xff;
    gpu->cmdPort = ((address>>8)&0x3f)|0b00000000;
    return gpu->rwPort ;
}
uint8_t readVramNext(){
    return gpu->rwPort;
}
void writeVramNext(uint8_t data){
    gpu->rwPort= data;
}

void outcharScreen(uint8_t c){}
void setPatNameTable(uint32_t address){
    address+=0x8000;
    uint8_t processedAddress = 0x7f&(address>>10);
    processedAddress |=0x1f;
    writeReg(2,processedAddress);
    
}

void waitVDPRdy(){
    while(readSReg(2)&1){

    }
}


void doHMMV(uint16_t x, uint16_t y,uint16_t lenX, uint16_t lenY, uint8_t color){
    waitVDPRdy();
    writeReg(36,x&0xff);
    writeReg(37,(x>>8)&1);
    writeReg(38,y&0xff);
    writeReg(39,(y>>8)&3);
    writeReg(40,lenX&0xff);
    writeReg(41,(lenX>>8)&1);
    writeReg(42,lenY&0xff);
    writeReg(43,(lenY>>8)&3);
    writeReg(44,color);
    writeReg(45,0);
    writeReg(46,0b11000000);
    waitVDPRdy();

}
void doHMMC(uint16_t x, uint16_t y,uint16_t lenX, uint16_t lenY, uint8_t firstByte){
    waitVDPRdy();
    writeReg(36,x&0xff);
    writeReg(37,(x>>8)&1);
    writeReg(38,y&0xff);
    writeReg(39,(y>>8)&3);
    writeReg(40,lenX&0xff);
    writeReg(41,(lenX>>8)&1);
    writeReg(42,lenY&0xff);
    writeReg(43,(lenY>>8)&3);
    writeReg(44,firstByte);
    writeReg(45,0);
    writeReg(46,0b11110000);
}
void doHMMM(uint16_t dX,uint16_t dY,uint16_t sX, uint16_t sY,uint16_t sizeX,uint16_t sizeY){
        waitVDPRdy();
    writeReg(32,sX&0xff);
    writeReg(33,(sX>>8)&1);
    writeReg(34,sY&0xff);
    writeReg(35,(sY>>8)&3);
    
    
    
    writeReg(36,dX&0xff);
    writeReg(37,(dX>>8)&1);
    writeReg(38,dY&0xff);
    writeReg(39,(dY>>8)&3);


    writeReg(40,sizeX&0xff);
    writeReg(41,(sizeX>>8)&1);
    writeReg(42,sizeY&0xff);
    writeReg(43,(sizeY>>8)&3);

    writeReg(44,0);
    writeReg(45,0);
    writeReg(46,0b11010000);
    waitVDPRdy();

}


void load8x8Tile(int tileMapIndex,const uint8_t* tile){
    int x = (tileMapIndex &0x1f)*8;
    int y = ((tileMapIndex>>5)*8)+512;
    doHMMC(x,y,8,8,*tile);
    for(int i = 1;i<32;i++){
        writeReg(44,tile[i]);
    }
}

void blit8x8Tile(int tileMapIndex,int targetX, int targetY){
    int x = (tileMapIndex &0x1f)*8;
    int y = ((tileMapIndex>>5)*8)+512;
    
    targetX*=8;
    targetY*=8;
    if(targetX>255){
        targetX-=255;
        targetY+=256;
    }
    doHMMM(targetX,targetY,x,y,8,8);

}





void clearTileLineBlit(int tileX){
    int x = tileX * 8;
    int y = 0;
    if(x>255){
        x-=255;
        y+=256;
    }
    int sizeX = 8;
    int sizeY = 212;
    doHMMV(x,y,sizeX,sizeY,0x44);
}





void clearScreen(uint32_t patternNameTable){
    doHMMV(0,0,255,511,0);
}




void setupPalletes(){
    int i = errno;
    for(int i =0;i<16;i++){
            writeReg(16,i);

        gpu->palPort = (gamePallete.colors[i].r<<4)|gamePallete.colors[i].b;
        gpu->palPort = gamePallete.colors[i].g;
    }
}

void setColGenTable(uint32_t address){
    writeReg(3,address>>6);
    writeReg(10,0x7&address>>14);
}

void setPattGenTable(uint32_t address){
    uint8_t processedAddress = (address>>11&0x3f)|3;
    writeReg(4,processedAddress);
    
}

void setColTable(uint32_t address){
    writeReg(3,(address>>6)|0x7f);
    writeReg(10,(address>>14&0x07));
}

void startGpu(){
    setupPalletes();
    writeReg(0,0b00000110);
    writeReg(1,0b01000010);
    
    
    writeReg(9,0b00000000);
    writeReg(7,0x01);
    writeReg(8,0b00001000);
    setPatNameTable(pattNTable);
    

    writeReg(12,0x0f);
    
    clearScreen(pattNTable);
    
             writeReg(19,0xe0);


}

void loadBigSprite(uint32_t spGenTable, uint8_t slot,const uint8_t* sprite){
    uint32_t loadAddr = spGenTable + slot*32;
    writeVram(loadAddr,*sprite);
    for(int i = 1;i<32;i++){
        writeVramNext(sprite[i]);
    }
}
void loadColorAttr(uint32_t coltable,uint8_t slot, const uint8_t* attr0){
        uint32_t loadAddr = coltable + slot*16;
        writeVram(loadAddr,*attr0);
        for(int i = 1;i<16;i++){
        writeVramNext(attr0[i]);
    }

}

void setAttribute(uint32_t attTable,uint8_t slot,uint8_t y,uint8_t x,uint8_t sprite){
    uint32_t loadAddr = attTable + slot*4;
    writeVram(loadAddr,y);
    writeVramNext(x);
    writeVramNext(sprite*4);
    writeVramNext(0);
}
void blitBlock(uint8_t x, uint8_t y,uint8_t bId){
    if(y%2)y--;
    screen[y/2][x/2] = bId;
    blit8x8Tile(blocks[bId].topLeft,x,y);
    blit8x8Tile(blocks[bId].topRight,x+1,y);
    blit8x8Tile(blocks[bId].bottLeft,x,y+1);
    blit8x8Tile(blocks[bId].bottRight,x+1,y+1);
}

void processBlockLine(uint16_t x,uint16_t mapPage){
    for(int y = 0 ; y<23;y+=4){
        blitBlock(x,y,1);
        if(x%4==0){
            blitBlock(x,y,2);
        }
    }

        
}

void processLine(uint8_t mapPage,uint16_t loadAddr){
    if(loadAddr%2==0){
        clearTileLineBlit(loadAddr);
        clearTileLineBlit(loadAddr+1);
    if(mapPage>18)mapPage=18;
    
    processBlockLine(loadAddr,mapPage);
        
    
    }
}

uint16_t screenMin;
uint16_t screenMax;
uint16_t loadAddr;
uint16_t lastScroll=0;
uint16_t lastImportantScroll;
uint8_t mapPage;
uint8_t oldLd;
void setHScroll(uint16_t scroll){
    
    scroll = scroll &0x1ff;
    screenMin = scroll/8;
    screenMax=screenMin+32;
    if(screenMax>=64)screenMax -=64;
    


    if(lastScroll<scroll){
        
        loadAddr = screenMax+16;
        if(loadAddr>63){
            
            loadAddr-=64;}
    }else{
        
        loadAddr = screenMin - 16;
        if(loadAddr>60000){
            loadAddr+=64;}
    }
    lastScroll = scroll;

    scroll = scroll^7;
    uint8_t charScroll = (scroll>>3)&0x3f;
    writeReg(26,charScroll);
    if(lastImportantScroll ==63 && charScroll==0){
        mapPage++;
    }else if(lastImportantScroll==0&&charScroll==63){
        mapPage--;
    }

    lastImportantScroll = charScroll;
    writeReg(27,(0x7&scroll));
    if(loadAddr!=oldLd){
        processLine(mapPage,loadAddr);
    }
    oldLd=loadAddr;


    printf("%d %d %d %d\n",mapPage,loadAddr,screenMin,screenMax);
}
void setVScroll(uint8_t scroll){
    writeReg(23,scroll);
}
uint16_t scroll = 0;
void gpuInt(){
    for(int i = 1; i<8;i++){
    scroll+=1;
    setHScroll(scroll);
    }
    writeReg(19,0xe0);
    return;
}
volatile  char *const UART_SRA = (volatile char *const) 0x800003;
volatile  char *const UART_TBD = (volatile char *const) 0x800007;

uint8_t hasChar(){
    return *UART_SRA&0b1;
}
char inChar(){
	return *UART_TBD;
}
void loadSprite(uint8_t slot,struct Sprite* sprite){
    loadBigSprite(0x18000,slot,sprite->mask0);
    loadColorAttr(coltable,slot,sprite->mask0);
    if(sprite->maskC>0){
        loadBigSprite(0x18000,slot+1,sprite->mask1);
        loadColorAttr(coltable,slot+1,sprite->attr1);
    }
    if(sprite->maskC>1){
        loadBigSprite(0x18000,slot+2,sprite->mask2);
        loadColorAttr(coltable,slot+2,sprite->attr2);
    }
}


int main(){
    blocks[1]=brickB;
    blocks[2]=questionB;
    setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
    printf("Loaded");
    startGpu();
    writeReg(25,0b00000011);
    //sprite gen table 
    writeReg(6,(0x18000>>11)&0x3f);
    



    //sprite attr table
    writeReg(5,(coltable>>7)|7);
    writeReg(11,(coltable>>15)&3);
    
   
    uint8_t x = 0;
    uint8_t y = 0;
    uint16_t scroll1=0;
        //
    load8x8Tile(0,&brick);
    load8x8Tile(1,&questionTl);
    load8x8Tile(2,&questionTr);
    load8x8Tile(3,&questionBl);
    load8x8Tile(4,&questionBr);
    loadSprite(0,&playerSprite);
    

    for(int index = 0;index<32;index++){
        writeVram(attable+(index*4),209);
        writeVramNext(0);
        writeVramNext(0);
        writeVramNext(0);
    }
//writeReg(0,0b00010110);
    uint8_t shouldBlit =0 ;
    while(1){
        //printf("Hello World!",errno);
        if(hasChar()){


        char c = inChar();
        switch (c)
        {
        case 'a':
            x--;
            break;
        case 'd':
            x++;
            break;
        case 'z':
            for(int i =0;i<8;i++){
                scroll1-=1;
                setHScroll(scroll1);
            }
            
    
            break;
        case 'c':
            for(int i =0;i<8;i++){
                scroll1+=1;
                setHScroll(scroll1);
            }
            break;    
        case 'w':
            y--;
            
            break;
        case 's':
            y++;
            break;
        case 'b':
            shouldBlit=1;
            break;
        default:
            break;
        }
        

        setAttribute(attable,0,y,x,0);
        setAttribute(attable,1,y,x,1);
        setAttribute(attable,2,y,x,2);
        uint8_t spriteTileX = (x+(scroll1%16))/16;
        spriteTileX+=screenMin/2;
        if(spriteTileX>=32)spriteTileX-=32;

        printf("Sprite tX:%d tY:%d\n",spriteTileX,(y+8)/16);
        printf("Tile at position: %d %d\n",screen[(y+8)/16][spriteTileX],scroll1%16);
        if(shouldBlit){
            
            blitBlock(spriteTileX*2,(y+8)/8,0);

            shouldBlit=0;
        }
        }

    }

}

