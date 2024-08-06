;
; File generated by cc65 v 2.19 - Git 925a589
;
	.fopt		compiler,"cc65 v 2.19 - Git 925a589"
	.setcpu		"65SC02"
	.smart		on
	.autoimport	on
	.case		on
	.debuginfo	off
	.importzp	sp, sreg, regsave, regbank
	.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
	.macpack	longbranch
	.forceimport	__STARTUP__
	.import		_initACIA
	.import		_putch
	.import		_getch
	.import		_puts
	.import		_print_hex
	.import		_SDSPI_readSector
	.import		_SDSPI_initSD
	.import		_SDSPI_writeMultStart
	.import		_SDSPI_writeMultContinue
	.import		_SDSPI_writeMultEnd
	.import		_printHex32
	.export		_buffer
	.export		_buffer1
	.export		_foo
	.export		_main

.segment	"RODATA"

S0004:
	.byte	$0A,$0D,$46,$61,$69,$6C,$65,$64,$20,$61,$74,$20,$73,$65,$63,$74
	.byte	$6F,$72,$20,$00
S0002:
	.byte	$0A,$0D,$54,$65,$73,$74,$20,$00
S0001	:=	S0002+0
S0006:
	.byte	$41,$6C,$69,$76,$65,$3F,$00
S0003:
	.byte	$0D,$0A,$6E,$6F,$00
S0007:
	.byte	$41,$6C,$6F,$00
S0005:
	.byte	$0A,$0D,$00

.segment	"BSS"

_buffer:
	.res	512,$00
_buffer1:
	.res	512,$00

; ---------------------------------------------------------------
; void __near__ foo (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_foo: near

.segment	"CODE"

	lda     #$00
	jsr     pusha
	jmp     incsp1

.endproc

; ---------------------------------------------------------------
; int __near__ main (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_main: near

.segment	"CODE"

	jsr     decsp5
	jsr     _foo
	lda     #$46
	jsr     _putch
	jsr     _SDSPI_initSD
	lda     #$00
	ldy     #$04
	sta     (sp),y
	lda     #$11
	sta     sreg+1
	lda     #$22
	sta     sreg
	lda     #$45
	ldx     #$33
	jsr     _printHex32
	ldy     #$02
	lda     #$00
	sta     (sp),y
	iny
	sta     (sp),y
	lda     #$46
	jsr     _putch
	lda     #$00
	sta     (sp)
	ldy     #$01
	sta     (sp),y
	lda     #$46
	jsr     _putch
	lda     #$46
	jsr     _putch
	lda     #$46
	jsr     _putch
	lda     #$46
	jsr     _putch
	lda     #<(S0001)
	ldx     #>(S0001)
	jsr     _puts
	lda     #$CA
	sta     _buffer+510
	lda     #$4F
	sta     _buffer+511
	lda     #$11
	sta     sreg+1
	lda     #$22
	sta     sreg
	lda     #$45
	ldx     #$33
	jsr     _printHex32
	lda     #<(S0002)
	ldx     #>(S0002)
	jsr     _puts
	stz     sreg+1
	stz     sreg
	lda     #$55
	ldx     #$05
	jsr     _SDSPI_writeMultStart
	cmp     #$00
	beq     L0002
	lda     #<(S0003)
	ldx     #>(S0003)
	jsr     _puts
	ldx     #$00
	jmp     L0017
L0002:	ldy     #$02
	sta     (sp),y
	iny
	sta     (sp),y
L0003:	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	cmp     #$19
	txa
	sbc     #$00
	bvc     L0007
	eor     #$80
L0007:	bpl     L0004
	lda     #<(_buffer)
	ldx     #>(_buffer)
	jsr     _SDSPI_writeMultContinue
	ldy     #$02
	ldx     #$00
	lda     #$01
	jsr     addeqysp
	bra     L0003
L0004:	lda     #<(_buffer)
	ldx     #>(_buffer)
	jsr     _SDSPI_writeMultContinue
	jsr     _SDSPI_writeMultEnd
	ldy     #$02
	lda     #$00
	sta     (sp),y
	iny
	sta     (sp),y
L0008:	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	cmp     #$19
	txa
	sbc     #$00
	bvc     L000C
	eor     #$80
L000C:	jpl     L0009
	iny
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	clc
	adc     #$55
	pha
	txa
	adc     #$05
	tax
	pla
	jsr     axlong
	jsr     pusheax
	lda     #<(_buffer1)
	ldx     #>(_buffer1)
	jsr     _SDSPI_readSector
	lda     #$00
	sta     (sp)
	ldy     #$01
	sta     (sp),y
L000D:	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	cmp     #$19
	txa
	sbc     #$00
	bvc     L0011
	eor     #$80
L0011:	bpl     L000A
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	sta     ptr1
	txa
	clc
	adc     #>(_buffer)
	sta     ptr1+1
	ldy     #<(_buffer)
	lda     (ptr1),y
	sta     sreg
	ldy     #$01
	lda     (sp),y
	tax
	lda     (sp)
	sta     ptr1
	txa
	clc
	adc     #>(_buffer1)
	sta     ptr1+1
	ldy     #<(_buffer1)
	lda     (ptr1),y
	cmp     sreg
	beq     L000F
	lda     #<(S0004)
	ldx     #>(S0004)
	jsr     _puts
	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	clc
	adc     #$55
	pha
	txa
	adc     #$05
	tax
	pla
	jsr     axlong
	jsr     _printHex32
	lda     #<(S0005)
	ldx     #>(S0005)
	jsr     _puts
L000F:	ldx     #$00
	lda     #$01
	jsr     addeq0sp
	bra     L000D
L000A:	iny
	ldx     #$00
	lda     #$01
	jsr     addeqysp
	jmp     L0008
L0009:	lda     #<(S0006)
	ldx     #>(S0006)
	jsr     _puts
	lda     #$55
	ldx     #$05
	jsr     push0ax
	lda     #<(_buffer)
	ldx     #>(_buffer)
	jsr     _SDSPI_readSector
	lda     _buffer+510
	jsr     _print_hex
	lda     _buffer+511
	jsr     _print_hex
	jsr     _initACIA
	lda     #<(S0007)
	ldx     #>(S0007)
	jsr     _puts
L0013:	jsr     _getch
	ldy     #$04
	sta     (sp),y
	ldx     #$00
	lda     (sp),y
	cmp     #$03
	beq     L0017
	jsr     _putch
	bra     L0013
L0017:	txa
	jmp     incsp5

.endproc
