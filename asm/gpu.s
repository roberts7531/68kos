        .globl scrollLine
.section .text
GPUCMD = 0xc00003
GPURW  = 0xc00001
# reg d0 val d1
_writeReg:
    move.b %d1,GPUCMD
    add #0x80,%d0
    move.b %d0,GPUCMD
    rts

    
scrollLine:
    move.b #14,%d0
    move.b #0,%d1;
    bsr _writeReg
    move.b #0,GPUCMD
    move.b #0x10,GPUCMD
    move.l #2159,%d1
    lea buffer,%a0
1:  move.b GPURW,(%a0)+
    dbra %d1,1b    

    move.b #14,%d0
    move.b #0,%d1;
    bsr _writeReg
    move.b #0,GPUCMD
    move.b #0x50,GPUCMD
    move.l #2000,%d1
    lea buffer+80,%a0
2:  move.b (%a0)+,GPURW
    dbra %d1,2b  
    move.l #80,%d1
3:  move.b #0,GPURW
    dbra %d1,3b
    rts 



.section .bss
buffer:   ds.b 2160