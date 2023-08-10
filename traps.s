    .globl _instVec

.section .text


UART_SRAA = 0x800003;
UART_TBDA = 0X800007;

_instVec:
      MOVEA.L #0xB8,%A0
      MOVE.L  #_trap14Handler,(%A0)+
      MOVE.L  #_trap15Handler,(%A0)
      RTS

_cout:
    btst.b #3,UART_SRAA
    beq.s _cout
    move.b %d1,UART_TBDA
    rts
_cin:
    btst.b #0,UART_SRAA
    beq _cin
    move.b UART_TBDA,%D1
    rts
_cins:
    btst.b #0,UART_SRAA
    beq 1f
    move.b #1,%d1
    rts
1:  move.b #0,%d1
    rts
_trap15Handler: 
            CMP.B #5,%D0
            BNE 1f
            bsr _cin
            rte 
1:          CMP.B #6,%D0
            BNE 2f
            bsr _cout
            rte
2:          cmp.b #7,%d0
            bne 3f
            bsr _cins
            rte
3:          rte     



# cpm bios calls remapped to trap #14 %D0: 
#0 initialise
#2 constat
#3 conin
#4 conout
#5 diskread
#6 diskwrite

_trap14Handler: 
        CMP.B #0,%D0
        BEQ _binit
        CMP.B #2,%D0
        BEQ _constat
        CMP.B #3,%D0
        BEQ _conin
        CMP.B #4,%D0
        BEQ _conout
        CMP.B #5,%D0
        BEQ _diskRead
        CMP.B #6,%D0
        BEQ _diskWrite
        RTE


_binit:  
        bsr cpmInit;
        CLR.L     %D0
        RTE
_constat:
        MOVE.B (UART_SRAA),%D0
        ANDI.B #1,%D0
        RTE
_conin:
        BTST.B #0,(UART_SRAA)
        BEQ.S _conin
        MOVE.B (UART_TBDA),%D0
        RTE
_conout:   
        BTST.B #3,UART_SRAA
        BEQ.S _conout
        MOVE.B %D1,(UART_TBDA)
        RTE
_diskRead: # %D1.L LBA %D2.W LOW COUNT HIGH DISK %A0 DEST
        MOVE.L  %A0,-(%SP)
        move.L  %D2,-(%SP)
        MOVE.L  %D1,-(%SP)
        BSR cpmDiskRead
        ADD #0x0c,%SP
        RTE
_diskWrite:
        MOVE.L  %A0,-(%SP)
        MOVE.L  %A0,-(%SP)
        MOVE.W  %D2,-(%SP)
        SWAP    %D2
        MOVE.W  %D2,-(%SP)
        MOVE.L  %D1,-(%SP)
        BSR cpmDiskWrite
        ADD #0x10,%SP
        RTE
