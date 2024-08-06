; XMODEM/CRC Sender for the 65C02
;
; By Daryl Rictor Aug 2002
;
; A simple file transfer program to allow upload from the SBC to a 
; console device utilizing the x-modem/CRC transfer protocol.  Requires just
; under 1k of either RAM or ROM, 132 bytes of RAM for the receive buffer,
; and 8 bytes of zero page RAM for variable storage.
;
;**************************************************************************
; This implementation of XMODEM/CRC does NOT conform strictly to the 
; XMODEM protocol standard in that it (1) does not accurately time character
; reception or (2) fall back to the Checksum mode.

; (1) For timing, it uses a crude timing loop to provide approximate
; delays.  These have been calibrated against a 1MHz CPU clock.  I have
; found that CPU clock speed of up to 5MHz also work but may not in
; every case.  Windows HyperTerminal worked quite well at both speeds!
;
; (2) Most modern terminal programs support XMODEM/CRC which can detect a
; wider range of transmission errors so the fallback to the simple checksum
; calculation was not implemented to save space.
;**************************************************************************
;
; Files uploaded via XMODEM-CRC will have the load address contained in
; the first two bytes in little-endian format:  
;  FIRST BLOCK
;     offset(0) = lo(load start address),
;     offset(1) = hi(load start address)
;     offset(2) = data byte (0)
;     offset(n) = data byte (n-2)
;
; Subsequent blocks
;     offset(n) = data byte (n)
;
; One note, XMODEM send 128 byte blocks.  If the block of memory that
; you wish to save is smaller than the 128 byte block boundary, then
; the last block will be padded with zeros.  Upon reloading, the
; data will be written back to the original location.  In addition, the
; padded zeros WILL also be written into RAM, which could overwrite other
; data.   
;
;-------------------------- The Code ----------------------------
;
; zero page variables (adjust these to suit your needs)
;
;

.import popa, popax
.importzp tmp1
.export _XmodemSend, _XmodemInit


.segment "ZEROPAGE"

lastBlk: .res 1 		; flag for last block
blkno:   .res 1 		; block number 
errcnt:  .res 1 		; error counter 10 is the limit

crc:     .res 1 		; CRC lo byte  (two byte variable)
crch:    .res 1 		; CRC hi byte  

ptr:     .res 1 		; data pointer (two byte variable)
ptrh:    .res 1   		;   "    "

eofp:    .res 1 		; end of file address pointer (2 bytes)
eofph:   .res 1		    ;  "	"	"	"

retry:   .res 1		    ; retry counter 
retry2:  .res 1		    ; 2nd counter

.segment	"CODE"
_WTF:
	rts

_XmodemInit:
	jmp MAKECRCTABLE

_XmodemSend:
    sta tmp1
    stx tmp1+1       ;len in tmp1
    
    jsr popax       

    clc
    adc tmp1
    sta ptr
    txa
    adc tmp1+1
    sta ptrh

    jsr XModem

    rts


;
;
; non-zero page variables and buffers
;
;
.org $300
Rbuff:  .res 133      	; temp 133 byte receive buffer 
					;(place anywhere, page aligned)
;
;
;  tables and constants
;
;
; The crclo & crchi labels are used to point to a lookup table to calculate
; the CRC for the 128 byte data blocks.  There are two implementations of these
; tables.  One is to use the tables included (defined towards the end of this
; file) and the other is to build them at run-time.  If building at run-time,
; then these two labels will need to be un-commented and declared in RAM.
;
;crclo		=	$7a00      	; Two 256-byte tables for quick lookup
;crchi		= 	$7b00      	; (should be page-aligned for speed)
;
;
;
; XMODEM Control Character Constants
SOH		=	$01		; start block
EOT		=	$04		; end of text marker
ACK		=	$06		; good block acknowledged
NAK		=	$15		; bad block acknowledged
CAN		=	$18		; cancel (not standard, not supported)
CR		=	$0d		; carriage return
LF		=	$0a		; line feed
ESC		=	$1b		; ESC to exit

;
;^^^^^^^^^^^^^^^^^^^^^^ Start of Program ^^^^^^^^^^^^^^^^^^^^^^
;
; Xmodem/CRC download routine
; By Daryl Rictor, August 8, 2002
;
; v0.1  code creation
;
;



.segment	"CODE"		; Start of program (adjust to your needs)







;
; Enter this routine with the beginning address stored in the zero page address
; pointed to by ptr & ptrh and the ending address stored in the zero page address
; pointed to by eofp & eofph.
;
;

XModem:		jsr	PrintMsg	; send prompt and info
		lda	#$00		;
		sta	errcnt		; error counter set to 0
		sta	lastBlk		; set flag to false
		lda	#$01		;
		sta	blkno		; set block # to 1
Wait4CRC:	lda	#$ff		; 3 seconds
		sta	retry2		;
		jsr	GetByte		;
		bcc	Wait4CRC	; wait for something to come in...
		cmp	#'C'		; is it the "C" to start a CRC xfer?
		beq	SetstAddr	; yes
		cmp	#ESC		; is it a cancel? <Esc> Key
		bne	Wait4CRC	; No, wait for another character
		jmp	PrtAbort	; Print abort msg and exit
SetstAddr:	ldy	#$00		; init data block offset to 0
		ldx	#$04		; preload X to Receive buffer
		lda	#$01		; manually load Blk #1	
		sta	Rbuff		; into 1st byte
		lda	#$FE		; load 1's comp of block #	
		sta	Rbuff+1		; into 2nd byte
		lda	ptr		; load low byte of start address		
		sta	Rbuff+2		; into 3rd byte	
		lda	ptrh		; load hi byte of start address		
		sta	Rbuff+3		; into 4th byte
		jmp	LdBuff1		; jump into buffer load routine

LdBuffer:	lda	lastBlk		; Was the last block sent?
		beq	LdBuff0		; no, send the next one	
		jmp 	Done		; yes, we're done
LdBuff0:		ldx	#$02		; init pointers
		ldy	#$00		;
		inc	blkno		; inc block counter
		lda	blkno		; 
		sta	Rbuff		; save in 1st byte of buffer
		eor	#$FF		; 
		sta	Rbuff+1		; save 1's comp of blkno next

LdBuff1:		lda	(ptr),y		; save 128 bytes of data
		sta	Rbuff,x		;
LdBuff2:		sec			; 
		lda	eofp		;
		sbc	ptr		; Are we at the last address?
		bne	LdBuff4		; no, inc pointer and continue
		lda	eofph		;
		sbc	ptrh		;
		bne	LdBuff4		; 
		inc	lastBlk		; Yes, Set last byte flag
LdBuff3:		inx			;
		cpx	#$82		; Are we at the end of the 128 byte block?
		beq	CalcCRC		; Yes, calc CRC
		lda	#$00		; Fill rest of 128 bytes with $00
		sta	Rbuff,x		;
		beq	LdBuff3		; Branch always

LdBuff4:		inc	ptr		; Inc address pointer
		bne	LdBuff5		;
		inc	ptrh		;
LdBuff5:		inx			;
		cpx	#$82		; last byte in block?
		bne	LdBuff1		; no, get the next

CalcCRC:		lda	#$00		; yes, calculate the CRC for the 128 bytes
		sta	crc		;
		sta	crch		;
		ldy	#$02		;
CalcCRC1:	lda	Rbuff,y		;
		jsr	UpdCRC		;		
		iny			;
		cpy	#$82		; done yet?
		bne	CalcCRC1	; no, get next
		lda	crch		; save Hi byte of CRC to buffer
		sta	Rbuff,y		;
		iny			;
		lda	crc		; save lo byte of CRC to buffer
		sta	Rbuff,y		;
Resend:		ldx	#$00		;
		lda	#SOH		; Send start block command
		jsr	Put_chr		;
SendBlk:		lda	Rbuff,x		; Send 133 bytes in buffer to the console
		jsr	Put_chr		;
		inx			;
		cpx	#$84		; last byte?
		bne	SendBlk		; no, get next
		lda	#$FF		; yes, set 3 second delay 
		sta	retry2		; and
		jsr	GetByte		; Wait for Ack/Nack
		bcc	Seterror	; No chr received after 3 seconds, resend
		cmp	#ACK		; Chr received... is it:
		beq	LdBuffer	; ACK, send next block
		cmp	#NAK		; 
		beq	Seterror	; NAK, inc errors and resend
		cmp	#ESC		;
		beq	PrtAbort	; Esc pressed to abort
					; fall through to error counter
Seterror:	inc	errcnt		; Inc error counter
		lda	errcnt		; 
		cmp	#$0A		; are there 10 errors? (Xmodem spec for failure)
		bne	Resend		; no, resend block
PrtAbort:	jsr	Flush		; yes, too many errors, flush buffer,
		jmp	Print_Err	; print error msg and exit
Done:		Jmp	Print_Good	; All Done..Print msg and exit
;
;^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
;
; subroutines
;
;					;
GetByte:		lda	#$00		; wait for chr input and cycle timing loop
		sta	retry		; set low value of timing loop
StartCrcLp:	jsr	Get_chr		; get chr from serial port, don't wait 
		bcs	GetByte1	; got one, so exit
		dec	retry		; no character received, so dec counter
		bne	StartCrcLp	;
		dec	retry2		; dec hi byte of counter
		bne	StartCrcLp	; look for character again
		clc			; if loop times out, CLC, else SEC and return
GetByte1:	rts			; with character in "A"
;
Flush:		lda	#$70		; flush receive buffer
		sta	retry2		; flush until empty for ~1 sec.
Flush1:		jsr	GetByte		; read the port
		bcs	Flush		; if chr recvd, wait for another
		rts			; else done
;
PrintMsg:	ldx	#$00		; PRINT starting message
PrtMsg1:		lda   	Msg,x		
		beq	PrtMsg2			
		jsr	Put_chr
		inx
		bne	PrtMsg1
PrtMsg2:		rts
Msg:		.byte	"Begin XMODEM/CRC transfer.  Press <Esc> to abort..."
		.BYTE  	CR, LF
               	.byte   0
;
Print_Err:	ldx	#$00		; PRINT Error message
PrtErr1:		lda   	ErrMsg,x
		beq	PrtErr2
		jsr	Put_chr
		inx
		bne	PrtErr1
PrtErr2:		rts
ErrMsg:		.byte 	"Download Error!"
		.BYTE  	CR, LF
                .byte   0
;
Print_Good:	ldx	#$00		; PRINT Good Transfer message
Prtgood1:	lda   	GoodMsg,x
		beq	Prtgood2
		jsr	Put_chr
		inx
		bne	Prtgood1
Prtgood2:	rts
GoodMsg:		.byte	EOT,EOT,EOT,CR,LF,CR,LF   ; send end of file three times
		.byte 	"Download Successful!"
		.BYTE  	CR, LF
                .byte   0
;
;
;======================================================================
;  I/O Device Specific Routines
;
;  Two routines are used to communicate with the I/O device.
;
; "Get_Chr" routine will scan the input port for a character.  It will
; return without waiting with the Carry flag CLEAR if no character is
; present or return with the Carry flag SET and the character in the "A"
; register if one was present.
;
; "Put_Chr" routine will write one byte to the output port.  Its alright
; if this routine waits for the port to be ready.  its assumed that the 
; character was send upon return from this routine.
;
; Here is an example of the routines used for a standard 6551 ACIA.
; You would call the ACIA_Init prior to running the xmodem transfer
; routine.
;
.import  ACIA_add, _ACIA_load_data, _ACIA_load_stat, _ACIA_set_reg, _ACIA_asm_init, _ACIA_store_data
ACIA_IO_data    = $00
ACIA_IO_stat    = $01
ACIA_IO_cmd     = $02
ACIA_IO_ctrl    = $03

;
; input chr from ACIA (no waiting)
;



Get_chr:		

;
; output to OutPut Port
;
.export _ACIAputChar, _ACIAgetChar



Put_chr:	   	jmp _ACIAputChar


ACIA_send_wait:
    ldy #4		               ;we have 4mhz so 4 times
ACIA_send_wait_loop_1:
    ldx #100             	   ;per megaherz
ACIA_send_wait_loop_0:
    dex
    nop
    cpx #00
    bne ACIA_send_wait_loop_0
    dey
    cpy #00
    bne ACIA_send_wait_loop_1

    rts

	
_ACIAgetChar:
	
	
	jsr _ACIA_load_stat
	;sta tmp1
	and #$08
    beq _ACIAgetChar


	jsr _ACIA_load_data
	;lda tmp1
    ldx #0





    rts

_ACIAputChar:

	

	jsr _ACIA_store_data

	jsr ACIA_send_wait


	rts
;=========================================================================
;
;
;  CRC subroutines 
;
;
UpdCRC:		eor 	crc+1 		; Quick CRC computation with lookup tables
       		tax		 	; updates the two bytes at crc & crc+1
       		lda 	crc		; with the byte send in the "A" register
       		eor 	CRCHI,X
       		sta 	crc+1
      	 	lda 	CRCLO,X
       		sta 	crc
       		rts
;
; Alternate solution is to build the two lookup tables at run-time.  This might
; be desirable if the program is running from ram to reduce binary upload time.
; The following code generates the data for the lookup tables.  You would need to
; un-comment the variable declarations for crclo & crchi in the Tables and Constants
; section above and call this routine to build the tables before calling the
; "xmodem" routine.
;
MAKECRCTABLE:
		ldx 	#$00
		LDA	#$00
zeroloop:	sta 	CRCLO,x
		sta 	CRCHI,x
		inx
		bne	zeroloop
		ldx	#$00
fetch:		txa
		eor	CRCHI,x
		sta	CRCHI,x
		ldy	#$08
fetch1:		asl	CRCLO,x
		rol	CRCHI,x
		bcc	fetch2
		lda	CRCHI,x
		eor	#$10
		sta	CRCHI,x
		lda	CRCLO,x
		eor	#$21
		sta	CRCLO,x
fetch2:		dey
		bne	fetch1
		inx
		bne	fetch
		rts

; The following tables are used to calculate the CRC for the 128 bytes
; in the xmodem data blocks.  You can use these tables if you plan to 
; store this program in ROM.  If you choose to build them at run-time, 
; then just delete them and define the two labels: crclo & crchi.
;
; low byte CRC lookup table (should be page aligned)

.segment "XTABLE"
CRCLO = $7a00


; hi byte CRC lookup table (should be page aligned)

CRCHI = $7b00

;
;
; End of File
;