IRQ_VECTOR      = $7ffd

ACIA_add    = $0400

ACIA_IO_data    = $00
ACIA_IO_stat    = $01
ACIA_IO_cmd     = $02
ACIA_IO_ctrl    = $03
ACIA_IO_write   = $00
ACIA_IO_read    = $04
asm_val_1       = $ff
ACIA_IO_ADD = __IO1PORTB__



.export _setIrqErs, _cli, _getKeyCode, _getKeyMap, _getKeyMapShifted, _peek, _poke, _ACIA_asm_init, _ACIA_load_data, _ACIA_load_stat, _ACIA_set_reg, _ACIA_store_data ; prepend labels with _ so C can see them
.export ACIA_add, ACIA_IO_data, ACIA_IO_stat, ACIA_IO_cmd, ACIA_IO_ctrl,_isVDPIRQ, asm_val_1
.import __IO0PORTB__,__AcciaAdd__, __IO1PORTA__, __IO1PORTB__, __IO1DDRA__, __IO1DDRB__
.import   _interupt
.import  __VIDADD__, __VIDVRAM__
.import popa, popax
.importzp ptr1, ptr2, tmp1, tmp2




.segment	"CODE"


_ACIA_load_stat:


    lda #ACIA_IO_stat|ACIA_IO_read
    jsr _ACIA_set_reg

    lda ACIA_add
    rts


_ACIA_load_data:

    lda #ACIA_IO_data|ACIA_IO_read
    jsr _ACIA_set_reg

    lda ACIA_add
    rts

    nop
    nop
    nop
    nop
_ACIA_store_data:
    phx
    tax
    lda #ACIA_IO_data|ACIA_IO_write
    sta __IO1PORTB__
    stx ACIA_add
    txa
    plx
    rts


_ACIA_set_reg:
    sta __IO1PORTB__
    rts


_ACIA_asm_init:


    lda #$ff
    sta __IO1DDRB__



    lda #ACIA_IO_stat|ACIA_IO_write
    jsr _ACIA_set_reg   ;set RS0 and RS1 on ACIA so select register status

    lda #00
    sta ACIA_add       ;SOFT RESET ;Addres on what acia is located without register selection


    lda #ACIA_IO_ctrl|ACIA_IO_write
    jsr _ACIA_set_reg   ;set ACIA reg

    lda #$1f    ;comment if with input NOT TESTED
    sta ACIA_add

  
    lda #ACIA_IO_cmd|ACIA_IO_write
    jsr _ACIA_set_reg

    lda #$0b
    sta ACIA_add



    rts


_getKeyCode:


    lda __IO0PORTB__
    ldx #0

    rts


_setIrqErs:
    lda #$4c                ;JMP op code      
    sta IRQ_VECTOR
    lda #<IRQ_CODE
    sta IRQ_VECTOR+1
    lda #>IRQ_CODE
    sta IRQ_VECTOR+2

    rts

_cli:
    cli
    rts


IRQ_CODE:
    pha
    phx
    phy


    jsr _interupt

    ply
    plx
    pla
    rti

_getKeyMap:
;char in A

    tax
    cpx #$66
    beq key_backspace
    cpx #$5A
    beq key_enter
    lda key_map,x

    ldx #0
;return in A
    rts
_getKeyMapShifted:
    ;char in A

    tax
    cpx #$66
    beq key_backspace
    cpx #$5A
    beq key_enter
    lda key_map_shifted,x

    ldx #0
;return in A
    rts
key_backspace:
    lda #$08 ;in ascii
    ldx #$00
    rts

key_enter:
    lda #$0D
    ldx #$00
    rts

_serialWriteAddress:
    sta __AcciaAdd__
    rts



    rts
IO_START = $400
VRAM_ADD = IO_START|$32
VIDEO_ADD = VRAM_ADD+1


_isVDPIRQ:

    lda VIDEO_ADD
    and #$80
    bne YESVDPIRQ

    lda #$00
    ldx #$00

    rts


YESVDPIRQ:
    lda #$ff
    sta asm_val_1
    
    ldx #$00
    lda #$01

    rts


_peek:
    sta ptr1
    stx ptr1+1
    lda (ptr1)
    ldx #0
    rts

_poke:
    sta tmp1        ;data

    jsr popax       ;address
    sta ptr1
    stx ptr1+1

    lda tmp1
    sta (ptr1)
    rts

key_map:
    .byte "????????????? `?" ;00 - 0F
    .byte "?????q1???ysaw2?" ;10 - 1F
    .byte "?cxde43?? vftr5?" ;20 - 2F
    .byte "?nbhgz6???mju78?" ;30 - 3F
    .byte "?,kio09??.-lcp'?" ;40 - 4F
    .byte "??c?s+?????d?z??" ;50 - 5F
    .byte "?????????1?47???" ;60 - 6F
    .byte "0.2568???+3-*9??" ;70 - 7F
    .byte "????????????????" ;80 - 8F
    .byte "????????????????" ;90 - 9F
    .byte "????????????????" ;A0 - AF
    .byte "????????????????" ;B0 - BF
    .byte "????????????????" ;C0 - CF
    .byte "????????????????" ;D0 - DF
    .byte "????????????????" ;E0 - EF
    .byte "????????????????" ;F0 - FF

key_map_shifted:
    .byte "????????????? `?" ;00 - 0F
    .byte "?????Q!???YSAW",$22,"?" ;10 - 1F
    .byte "?CXDE$#?? VFTR%?" ;20 - 2F
    .byte "?NBHGZ&???MJU/(?" ;30 - 3F
    .byte "?;KIO=)??:_LCP??" ;40 - 4F
    .byte "??C?S*?????D?Z??" ;50 - 5F
    .byte "?????????!?$/???" ;60 - 6F
    .byte "=:2568???+3-*??9" ;70 - 7F
    .byte "????????????????" ;80 - 8F
    .byte "????????????????" ;90 - 9F
    .byte "????????????????" ;A0 - AF
    .byte "????????????????" ;B0 - BF
    .byte "????????????????" ;C0 - CF
    .byte "????????????????" ;D0 - DF
    .byte "????????????????" ;E0 - EF
    .byte "????????????????" ;F0 - FF

