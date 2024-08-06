.segment	"CODE"

ACIA_DATA   = $0400
ACIA_STATUS = $0401
ACIA_CMD    = $0402
ACIA_CTRL   = $0403

ACIA_RX = ACIA_DATA
ACIA_TX = ACIA_DATA

SYS_IO      = $04F0
SYS_PORTB   = $04F0
SYS_PORTA   = $04F1
SYS_DDRB    = $04F2
SYS_DDRA    = $04F3

PORT_B = SYS_PORTB 
PORT_A = SYS_PORTA 
DDRB =  SYS_DDRB  
DDRA =  SYS_DDRA  

ACIA = $0400       ; BASE ADDRESS OF 6551 ACIA

ACIAD   = ACIA+0      ; ACIA DATA
ACIAS   = ACIA+1      ; ACIA STATUS
ACIACMD = ACIA+2      ; ACIA COMMAND
ACIACTL = ACIA+3      ; ACIA CONTROL

;SPI bit mask
SPI0_DATA_BIT_IN  = %00001000
SPI0_DATA_BIT_OUT = %00000001
SPI0_CLK_BIT      = %00000010
SPI0_CE_BIT       = %00000100
SPI0_CE1_BIT       = %00010000


.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4, sreg

.import popa, _print_hex, _putch
.segment	"BSS"
btmp0:
	.res	4
.segment "CODE"
r1 = tmp1
r0 = tmp2
SD_tmp_cnt   = tmp1
SD_bufferPtr = ptr1      
SD_bufferPtrHi = ptr1+1
zp_sd_cmd_address = tmp2

PORTA = PORT_A
SD_CS   = SPI0_CE1_BIT
SD_SCK  = SPI0_CLK_BIT
SD_MOSI = SPI0_DATA_BIT_OUT
SD_MISO = SPI0_DATA_BIT_IN

PORTA_OUTPUTPINS = SD_CS | SD_SCK | SD_MOSI

;.import _putch, _print_hex

.export _SDSPI_readSector, _SDSPI_initSD, _SDSPI_writeSector, _SDSPI_readMultStart, _SDSPI_readMultContinue, _SDSPI_readMultEnd,  _SDSPI_writeMultStart, _SDSPI_writeMultContinue, _SDSPI_writeMultEnd




print_char = _putch
;SD card stuff
print_hex = _print_hex


delay:
  ldx #0
  ldy #0
@loop:
  dey
  bne @loop
  dex
  bne @loop
  rts
_SDSPI_readSector:
SD_ReadSingleSector:
;Must do this because of C
  sta SD_bufferPtr
  stx SD_bufferPtr+1
  jsr popa
  sta btmp0
  jsr popa
  sta btmp0+1
  jsr popa
  sta btmp0+2
  jsr popa
  sta btmp0+3

  
  ; Read a sector
  ;lda #'r'
  ;jsr print_char
  ;lda #'s'
  ;jsr print_char
  ;lda #':'
  ;jsr print_char

  lda #SD_MOSI
  sta PORTA

  ; Command 17, arg is sector number, crc not checked
  lda #$51           ; CMD17 - READ_SINGLE_BLOCK
  jsr sd_writebyte
  lda btmp0+3           ; sector 24:31
  jsr sd_writebyte
  lda btmp0+2           ; sector 16:23
  jsr sd_writebyte
  lda btmp0+1           ; sector 8:15
  jsr sd_writebyte
  lda btmp0           ; sector 0:7
  jsr sd_writebyte
  lda #$01           ; crc (not checked)
  jsr sd_writebyte

  jsr sd_waitresult
  cmp #$00
  beq @readsuccess
  
  lda #'f'
  jsr print_char
  sec
  lda #$01
  ldx #$00
  rts

@readsuccess:
  
  ;lda #'s'
  ;jsr print_char
  ;lda #':'
  ;jsr print_char

  ; wait for data
  jsr sd_waitresult
  cmp #$fe
  beq @readgotdata
  jsr print_hex
  lda #'f'
  jsr print_char
  sec
  lda #$02
  ldx #$00
  rts

@readgotdata:
  ; Need to read 512 bytes.  Read two at a time, 256 times.
  ;also because of C pointer is in registers A and X

  stz SD_tmp_cnt ; counter
  ldy #$00
@readloop:
  phy
  jsr sd_readbyte
  ply
  sta (SD_bufferPtr),Y ; byte1
  iny
  dec SD_tmp_cnt ; counter
  bne @readloop

  inc SD_bufferPtrHi
  stz SD_tmp_cnt ; counter
  ldy #$00
@readloop1:
  phy
  jsr sd_readbyte
  ply
  sta (SD_bufferPtr),Y ; byte1
  iny
  dec SD_tmp_cnt ; counter
  bne @readloop1



  ; End command
  lda #SD_CS | SD_MOSI
  sta PORTA

  ; Print the last two bytes read, in hex
  ;lda $01 ; byte1
  ;jsr print_hex
  ;lda $02 ; byte2
  ;jsr print_hex
  clc
  lda #$01
  ldx #$00
  rts
_SDSPI_writeSector:
SD_writeSingleSector:
  sta SD_bufferPtr  ;because cc65
  stx SD_bufferPtr+1
  jsr popa
  sta btmp0
  jsr popa
  sta btmp0+1
  jsr popa
  sta btmp0+2
  jsr popa
  sta btmp0+3
  ; Write a sector
  ;lda #'w'
  ;jsr print_char
  ;lda #'s'
  ;jsr print_char
  ;lda #':'
  ;jsr print_char

  lda #SD_MOSI
  sta PORTA

  ; Command 24, arg is sector number, crc not checked
  lda #$58           ; CMD24 - WRITE_SINGLE_BLOCK
  jsr sd_writebyte
  lda btmp0+3           ; sector 24:31
  jsr sd_writebyte
  lda btmp0+2           ; sector 16:23
  jsr sd_writebyte
  lda btmp0+1           ; sector 8:15
  jsr sd_writebyte
  lda btmp0           ; sector 0:7
  jsr sd_writebyte
  lda #$01           ; crc (not checked)
  jsr sd_writebyte

  jsr sd_waitresult
  cmp #$00
  beq @writesuccess

@SD_W_FIAL:
  sec
  lda #$01
  ldx #$00
  rts

@writesuccess:
  ;lda #'s'
  ;jsr print_char
  ;lda #':'
  ;jsr print_char

  ;; wait for data
  ;jsr sd_waitresult
  ;cmp #$fe
  ;beq .writrgotdata
  lda #$fe
  jsr sd_writebyte  ;data token
;;
  ;lda #'f'
  ;jsr print_char
  ;jmp loop

@writrgotdata:
  ; Need to write 512 bytes.  Read two at a time, 256 times.

  ldy #$00
  stz SD_tmp_cnt ; counter
@writeloop:
  lda (SD_bufferPtr),Y
  phy
  jsr sd_writebyte
  ply
  iny
  dec SD_tmp_cnt ; counter
  bne @writeloop

  inc SD_bufferPtrHi
  ldy #$00
  stz SD_tmp_cnt ; counter
@writeloop1:
  lda (SD_bufferPtr),Y
  phy
  jsr sd_writebyte
  ply
  iny
  dec SD_tmp_cnt ; counter
  bne @writeloop1


  lda #$00
  jsr sd_writebyte
  lda #$00
  jsr sd_writebyte  ;CRC maybe not chekd

  ;lda #'Y'
  ;jsr acia_print_char

  jsr sd_waitresult
  cmp #$E5
  bne @SD_W_FIAL
  ;jsr print_hex
  jsr delay
  jsr sd_waitresult
  cmp #$00
  bne @SD_W_FIAL
  ;jsr print_hex
  jsr delay

  
  ; End command
  lda #SD_CS | SD_MOSI
  sta PORTA
  clc
  lda #$00
  ldx #$00
  rts
 


_SDSPI_initSD:
  
sd_init:

  ; Let the SD card boot up, by pumping the clock with SD CS disabled

  ; We need to apply around 80 clock pulses with CS and MOSI high.
  ; Normally MOSI doesn't matter when CS is high, but the card is
  ; not yet is SPI mode, and in this non-SPI state it does care.

  lda #PORTA_OUTPUTPINS   ; Set various pins on port A to output
  sta DDRA

  lda #SD_CS | SD_MOSI
  ldx #160               ; toggle the clock 160 times, so 80 low-high transitions
  
@preinitloop:
  eor #SD_SCK
  sta PORTA
  dex
  bne @preinitloop
  

@cmd0: ; GO_IDLE_STATE - resets card to idle state, and SPI mode
  lda #<cmd0_bytes
  sta zp_sd_cmd_address
  lda #>cmd0_bytes
  sta zp_sd_cmd_address+1

  jsr sd_sendcommand
    
  ; Expect status response $01 (not initialized)
  cmp #$01
  bne @initfailed
  
@cmd8: ; SEND_IF_COND - tell the card how we want it to operate (3.3V, etc)
  lda #<cmd8_bytes
  sta zp_sd_cmd_address
  lda #>cmd8_bytes
  sta zp_sd_cmd_address+1

  jsr sd_sendcommand
  
  ; Expect status response $01 (not initialized)
  cmp #$01
  bne @initfailed
  
  ; Read 32-bit return value, but ignore it
  jsr sd_readbyte
  jsr sd_readbyte
  jsr sd_readbyte
  jsr sd_readbyte
  
@cmd55: ; APP_CMD - required prefix for ACMD commands
  lda #<cmd55_bytes
  sta zp_sd_cmd_address
  lda #>cmd55_bytes
  sta zp_sd_cmd_address+1

  jsr sd_sendcommand

  ; Expect status response $01 (not initialized)
  cmp #$01
  bne @initfailed

@cmd41: ; APP_SEND_OP_COND - send operating conditions, initialize card
  lda #<cmd41_bytes
  sta zp_sd_cmd_address
  lda #>cmd41_bytes
  sta zp_sd_cmd_address+1

  jsr sd_sendcommand

  ; Status response $00 means initialised
  cmp #$00
  beq @initialized

  ; Otherwise expect status response $01 (not initialized)
  cmp #$01
  bne @initfailed

  ; Not initialized yet, so wait a while then try again.
  ; This retry is important, to give the card time to initialize.
  jsr delay
  jmp @cmd55


@initialized:
  lda #'Y'
  jsr print_char
  clc
  lda #$00
  ldx #$00
  rts

@initfailed:
  lda #'X'
  jsr print_char
  sec
  lda #$01
  ldx #$00
  rts


cmd0_bytes:
  .byte $40, $00, $00, $00, $00, $95
cmd8_bytes:
  .byte $48, $00, $00, $01, $aa, $87
cmd12_bytes:
  .byte $4C, $00, $00, $00, $00, $00
cmd55_bytes:
  .byte $77, $00, $00, $00, $00, $01
cmd41_bytes:
  .byte $69, $40, $00, $00, $00, $01



sd_readbyte:
  ; Enable the card and tick the clock 8 times with MOSI high, 
  ; capturing bits from MISO and returning them

  ldx #8                      ; we'll read 8 bits
@loop:

  lda #SD_MOSI                ; enable card (CS low), set MOSI (resting state), SCK low
  sta PORTA

  lda #SD_MOSI | SD_SCK       ; toggle the clock high
  sta PORTA

  lda PORTA                   ; read next bit
  and #SD_MISO

  clc                         ; default to clearing the bottom bit
  beq @bitnotset              ; unless MISO was set
  sec                         ; in which case get ready to set the bottom bit
@bitnotset:

  tya                         ; transfer partial result from Y
  rol                         ; rotate carry bit into read result
  tay                         ; save partial result back to Y

  dex                         ; decrement counter
  bne @loop                   ; loop if we need to read more bits

  rts


sd_writebyte:
  ; Tick the clock 8 times with descending bits on MOSI
  ; SD communication is mostly half-duplex so we ignore anything it sends back here

  ldx #8                      ; send 8 bits

@loop5:
  asl                         ; shift next bit into carry
  tay                         ; save remaining bits for later

  lda #0
  bcc @sendbit                ; if carry clear, don't set MOSI for this bit
  ora #SD_MOSI

@sendbit:
  sta PORTA                   ; set MOSI (or not) first with SCK low
  eor #SD_SCK
  sta PORTA                   ; raise SCK keeping MOSI the same, to send the bit

  tya                         ; restore remaining bits to send

  dex
  bne @loop5                   ; loop if there are more bits to send

  rts


sd_waitresult:
  ; Wait for the SD card to return something other than $ff
  jsr sd_readbyte
  cmp #$ff
  beq sd_waitresult
  rts


sd_sendcommand:
  ; Debug print which command is being executed
    
  ;lda #'c'
  ;jsr print_char
  ldx #0
  ;lda (zp_sd_cmd_address,x)
  ;jsr print_hex

  lda #SD_MOSI           ; pull CS low to begin command
  sta PORTA

  ldy #0
  lda (zp_sd_cmd_address),y    ; command byte
  
  jsr sd_writebyte
  ldy #1
  lda (zp_sd_cmd_address),y    ; data 1
  jsr sd_writebyte
  ldy #2
  lda (zp_sd_cmd_address),y    ; data 2
  jsr sd_writebyte
  ldy #3
  lda (zp_sd_cmd_address),y    ; data 3
  jsr sd_writebyte
  ldy #4
  lda (zp_sd_cmd_address),y    ; data 4
  jsr sd_writebyte
  ldy #5
  lda (zp_sd_cmd_address),y    ; crc
  jsr sd_writebyte

  jsr sd_waitresult
  pha

  ; Debug print the result code
  ;jsr print_hex

  ; End command
  lda #SD_CS | SD_MOSI   ; set CS high again
  sta PORTA

  pla   ; restore result code
  rts









_SDSPI_readMultStart:
  sta tmp1
  stx tmp2

  lda #SD_MOSI
  sta PORTA

  ; Command 18, arg is sector number, crc not checked
  lda #$52           ; CMD18 - READ_MULTIPLE_BLOCK
  jsr sd_writebyte
  lda sreg+1           ; sector 24:31
  jsr sd_writebyte
  lda sreg           ; sector 16:23
  jsr sd_writebyte
  lda tmp2          ; sector 8:15
  jsr sd_writebyte
  lda tmp1           ; sector 0:7
  jsr sd_writebyte
  lda #$01           ; crc (not checked)
  jsr sd_writebyte

  jsr sd_waitresult
  cmp #$00
  beq @readsuccess
  
  lda #'f'
  jsr print_char
  lda #$01
  ldx #$00
  rts

@readsuccess:
  lda #$00
  ldx #$00
  rts

_SDSPI_readMultContinue:
  sta SD_bufferPtr  ;cc65
  stx SD_bufferPtrHi

  jsr sd_waitresult
  cmp #$fe
  beq @readMultgotdata
  lda #'f'
  jsr print_char
  lda #$02
  ldx #$00
  rts
@readMultgotdata:

  ; Need to read 512 bytes.  Read two at a time, 256 times.
  ;also because of C pointer is in registers r0 and r1

  stz SD_tmp_cnt ; counter
  ldy #$00
@readloop:
  phy
  jsr sd_readbyte
  ply
  sta (SD_bufferPtr),Y ; byte1
  iny
  dec SD_tmp_cnt ; counter
  bne @readloop

  inc SD_bufferPtrHi
  stz SD_tmp_cnt ; counter
  ldy #$00
@readloop1:
  phy
  jsr sd_readbyte
  ply
  sta (SD_bufferPtr),Y ; byte1
  iny
  dec SD_tmp_cnt ; counter
  bne @readloop1

  jsr sd_readbyte
  jsr sd_readbyte ;CRC not cheched
  lda #$00
  ldx #$00
  rts


_SDSPI_readMultEnd:
  lda #<cmd12_bytes
  sta zp_sd_cmd_address
  lda #>cmd12_bytes
  sta zp_sd_cmd_address+1

  jsr sd_sendcommand

  cmp #$00
  bne @endFail
  ldx #$00
  rts
@endFail:
  lda #$03
  lda #'f'
  jsr print_char
  ldx #$00
  rts

_SDSPI_writeMultStart:
  sta tmp1
  stx tmp2

  lda #SD_MOSI
  sta PORTA

  ; Command 18, arg is sector number, crc not checked
  lda #$59           ; CMD18 - READ_MULTIPLE_BLOCK
  jsr sd_writebyte
  lda sreg+1             ; sector 24:31
  jsr sd_writebyte
  lda sreg             ; sector 16:23
  jsr sd_writebyte
  lda tmp2           ; sector 8:15
  jsr sd_writebyte
  lda tmp1           ; sector 0:7
  jsr sd_writebyte
  lda #$01           ; crc (not checked)
  jsr sd_writebyte

  jsr sd_waitresult
  cmp #$00
  beq @writesuccess
  jsr delay
  lda #'f'
  jsr print_char
  lda #$01
  ldx #$00
  rts

@writesuccess:
  lda #$00
  ldx #$00
  rts



_SDSPI_writeMultContinue:
  phy
  sta SD_bufferPtr  ;cc65
  stx SD_bufferPtrHi

  lda #$fc
  jsr sd_writebyte  ;data token


@writrgotdata:
  ; Need to write 512 bytes.  Read two at a time, 256 times.

  ldy #$00
  stz SD_tmp_cnt ; counter
@writeloop:
  lda (SD_bufferPtr),Y
  phy
  jsr sd_writebyte
  ply
  iny
  dec SD_tmp_cnt ; counter
  bne @writeloop

  inc SD_bufferPtrHi
  ldy #$00
  stz SD_tmp_cnt ; counter
@writeloop1:
  lda (SD_bufferPtr),Y
  phy
  jsr sd_writebyte
  ply
  iny
  dec SD_tmp_cnt ; counter
  bne @writeloop1


  lda #$00
  jsr sd_writebyte
  lda #$00
  jsr sd_writebyte  ;CRC not checked

  ;lda #'Y'
  ;jsr acia_print_char

  jsr sd_waitresult
  cmp #$E5
  bne @SD_W_FIAL

@wmwait:
  jsr sd_readbyte
  cmp #0
  beq @wmwait
  
  lda #$00
  ply
  ldx #$00
  rts
@SD_W_FIAL:
  lda #'f'
  jsr print_char
  lda #$04
  ply
  ldx #$00
  rts
@SD_WA_FIAL:
  lda #'f'
  jsr print_char
  lda #$05
  ply
  ldx #$00
  rts

_SDSPI_writeMultEnd:
  lda #$FD
  jsr sd_writebyte
  jsr sd_waitresult

@wewait:
  jsr sd_readbyte
  cmp #0
  beq @wewait

  lda #$00
  ldx #$00
  rts