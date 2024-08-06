.import __IO1ADDDR__, __DDRA__, __IO1DDRA__, __IO1PORTA__
.import popa, popax
.importzp ptr1, ptr2, tmp1, tmp2
.export _FloppyTest,_FloppySendSerial,_FloppySetReceveSerial,_FloppySetSendSerial,_FloppyReceveSerial,_waite

_FloppyTest:



    jsr _FloppyReceveSerial

    ;lda #65
    rts




_waite:

    ldx #$ff
    ldy #$08

    jmp wait_loop_0
wait:
    ldx #$ff
    ldy #$04

wait_loop_0:
    dex
    bne wait_loop_0
    dey 
    bne wait_loop_0
    rts

chkRdy:

    ;lda __IO1PORTA__
    ;and #$01
    ;bne chkRdy


    rts

_FloppySendSerial:
    jsr chkRdy

    sta __IO1ADDDR__|$0A
    jsr wait
    rts


_FloppyReceveSerial:
 
    lda __IO1ADDDR__|$0A

    
FloppyReceveSerialloop:
    lda __IO1ADDDR__|$0D
    and #$12
    beq FloppyReceveSerialloop
    jsr wait


    lda __IO1ADDDR__|$0A
    ldx #$00
    rts


_FloppySetSendSerial:
    ;set for rdy line
    lda #$00
    sta __IO1DDRA__

     ;set IER register
    lda #$7F
    sta __IO1ADDDR__|$0E

    ;set T2 counter
    lda #$ff
    sta __IO1ADDDR__|$10
    lda #$ff
    sta __IO1ADDDR__|$10

    ;set ACR
    lda #$17
    sta __IO1ADDDR__|$0B

    lda #$02
    ;sta __IO1ADDDR__|$0D

    rts

_FloppySetReceveSerial:
    ;set for rdy line
    lda #$00
    sta __IO1DDRA__


     ;set IER register
    lda #$7F
    sta __IO1ADDDR__|$0E

    ;set T2 counter
    lda #$ff
    sta __IO1ADDDR__|$10
    lda #$ff
    sta __IO1ADDDR__|$10

    ;set ACR
    lda #$07
    sta __IO1ADDDR__|$0B

    ;sta __IO1ADDDR__|$0D

    rts