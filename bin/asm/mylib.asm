;.export __ACIA_DATA,__ACIA_STATUS,__ACIA_CMD,__ACIA_CTRL, __acia_init, __aciaPrintChar, __aciaGetChar, __testAcia
.segment	"CODE"
__ACIA_DATA   = $0400
__ACIA_STATUS = $0401
__ACIA_CMD    = $0402
__ACIA_CTRL   = $0403
.export _initACIA, _putch, _getch, _print_hex, __wait
print_char = _putch
_initACIA:

  LDA     #$0B            ; TRANSMIT AND RECEIVE, NO INTERRUPTS
  STA     __ACIA_CMD
  LDA     #$10            ; 19200 8 N 1, USE BAUD RATE GENERATOR
  STA     __ACIA_CTRL
  jsr __wait
  rts
_putch:

  sta __ACIA_DATA
  jsr __wait

  rts
;char in A
__aciaPrintChar:
  pha
  sta __ACIA_DATA
  jsr __wait
  pla
  rts
_getch:
  lda __ACIA_STATUS
  and #$08
  beq _getch
  lda __ACIA_DATA
  ldx #$00
  rts
__testAcia:
  lda #'B'
  jsr __aciaPrintChar
  rts


_print_hex:
  pha
  ror
  ror
  ror
  ror
  jsr print_nybble
  pla
print_nybble:
  and #15
  cmp #10
  bmi @skipletter
  adc #6
@skipletter:
  adc #48
  jsr print_char
  rts


__wait:
    phx
    phy
    ldx #$FF
    ldy #$05
l1:
    ldx #$80
l2:
    nop
    nop
    dex
    bne l2
    dey
    bne l1
    ply
    plx

    rts

__aciaGetChar:
  lda __ACIA_STATUS
  and #$08
  beq __aciaGetChar
  lda __ACIA_DATA
  rts