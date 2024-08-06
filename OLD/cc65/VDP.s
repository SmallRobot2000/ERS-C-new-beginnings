;-- VDP.s --

.export _VDP_W_REG ; prepend labels with _ so C can see them
.export _vdp_debug
.export _VDP_R_STAT
.export _VDP_W_VRAM
.export _VDP_W_BLOCK_asm

.import  __VIDADD__, __VIDVRAM__
.import popa, popax
.importzp ptr1, ptr2, tmp1, tmp2, asm_val_1

valmy = ptr1 ; borrow runtime zp pointers
regmy = ptr2 ; tmp1 and tmp2 are also available


.segment	"CODE"

_VDP_W_REG:
    sta valmy ; __fastcall__ so last arg is in A/X

    jsr popa  ; pull reg off stack
    tax       ; reg in X
    lda valmy ; val in A


    sta __VIDADD__
    txa
    clc
    ora #%10000000
    clc
    and #%10000111
    sta __VIDADD__


    rts
_VDP_R_STAT:

    lda VIDEO_ADD

    ldx #$00
    rts
_vdp_debug:


    jmp $C000
    lda #$aa
    ldx #$07

    sta VIDEO_ADD
    txa
    clc
    ora #%10000000
    clc
    and #%10000111
    sta VIDEO_ADD

    rts

_VDP_W_VRAM:

    pha     ;last in a/x
    jsr popax   
    phx
    tax
    pla
    tay
    pla


    pha
    phx
    phy

    pha ; A on stack
    tya
    and #%01111111
    ora #%01000000
    tay

    stx VIDEO_ADD
    nop
    nop
    
    
    sty VIDEO_ADD
    pla
    nop
    nop
   
    sta VRAM_ADD

    ply
    plx
    pla
    rts



    rts

_VDP_W_BLOCK_asm:
    sta tmp1 ;restore len in tmp1
    stx tmp1+1

    jsr popax

    sta ptr1
    stx ptr1+1 ;Push unsigned int Vram_ptr on ptr1

    jsr popax  

    sta ptr2
    stx ptr2+1 ;Put void Ram_ptr on ptr2


    lda ptr1
    sta VIDEO_ADD

    lda ptr1+1
    and #%01111111
    ora #%01000000


    sta VIDEO_ADD


    
    jsr waitVDP
_VDP_W_BLOCK_loop:

    
    
    lda (ptr2)
    sta VRAM_ADD

    inc ptr2        ;increment RAMptr
    bne LABELVDP_2
    inc ptr2+1
LABELVDP_2:


    lda tmp1        ;decrement len
    bne LABELVDP_0
    dec tmp1+1
LABELVDP_0:
    dec tmp1
    
    lda tmp1
    beq LABELVDP_1
    jmp _VDP_W_BLOCK_loop
LABELVDP_1:
    
    lda tmp1+1
    beq LABELVDP_3
    jmp _VDP_W_BLOCK_loop
LABELVDP_3:

    rts


waitVDP:
    sei
    lda asm_val_1
    cli
    beq waitVDP
    stz asm_val_1

    rts
IO_START = $400
VRAM_ADD = IO_START|$32
VIDEO_ADD = VRAM_ADD+1