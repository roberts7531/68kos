    .extern bss_start
    .extern bss_size
    .extern rom_data
    .extern data_start
    .extern data_size
    .extern _instVec
    .extern gpuInt
    .globl  _start
    .section .text.startup
DUART = 0x800000


MRA = DUART+1 
SRA = DUART+3 
CSRA = DUART+3 
CRA = DUART+5 
RBA = DUART+7 
TBA = DUART+7 
ACR = DUART+9 
ISR = DUART+11 
IMR = DUART+11 
CUR = DUART+13
CLR = DUART+15
MRB = DUART+17
SRB = DUART+19 
CSRB = DUART+19
CRB = DUART+21 
RBB = DUART+23 
TBB = DUART+23 
IVR = DUART+25 
OPRC = DUART+27
OPR   = DUART+29
_start:
    #dc.l 0xffffe
    #dc.l reals
reals:
        move.w   #0x2000,%SR
    #move.l #0,0xB00000
   # movea.l #0,%a0
    #move.l #0xffffe,(%a0)+
    #move.l #reals,(%a0)+
   # move.l #0xffffe,%sp

    
skipdata:
    movea.l #bss_start,%a0
    move.l #bss_size,%d0
    beq skipbss
1:  clr.b (%a0)+
    subq.l #1,%d0
    bne 1b
skipbss:

    #MOVE.B #0x30,CRA
    #MOVE.B #0x20,CRA
    #MOVE.B #0x10,CRA
    #move.b #0x0,IMR
	#move.b  #0x93, MRA
    #move.b  #0x07, MRA
	#move.b  #0x60, ACR       | Baud Rate Set #2

	#move.b CRA,%D0

    #move.b  #0x66, SRA      | Set Tx and Rx rates to 9600
    #move.b #0x0,CUR
    #move.b #0x2,CLR
    #move.b OPR,%d0

    #MOVE.B #0x05,CRA
    bsr installInterruptHandler
    jmp main


installInterruptHandler:
    MOVEA.L #0x64,%A0
      MOVE.L #handler,(%A0)+
      MOVE.L #handler,(%A0)+
      MOVE.L #handler,(%A0)+
      MOVE.L #handler,(%A0)+
      MOVE.L #handler,(%A0)+
      MOVE.L #handler,(%A0)+
      MOVE.L #handler,(%A0)+
    rts

handler:
    movem.L %d0-%d7/%a0-%a7, registerBackup
    bsr gpuInt
    movem.L registerBackup,%d0-%d7/%a0-%a7
    rte

    .section .bss
registerBackup: ds.l 16