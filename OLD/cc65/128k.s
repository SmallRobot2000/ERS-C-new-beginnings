.import __PORTA__, __DDRA__
.import popa, popax
.importzp ptr1, ptr2, tmp1, tmp2
.export _putByteEMem, _getByteEMem, _EMem_int

.segment	"CODE"


_EMem_int:
    lda #$ff
    sta __DDRA__
    rts

_putByteEMem:
;(char data, char bank, unsignet int address)


    sta ptr1
    stx ptr1+1      ;Save address

    jsr popa        ;Get bank
    ora #$80        
    and #$8f        ;make shure that nothing hapends with other I/O devices
    sta __PORTA__

    jsr popa        ;get data
    pha

    lda ptr1+1
    ora #$80
    and #$BF        ; make sure it starts at $8000 and ends at $bfff
    sta ptr1+1

    pla
    sta (ptr1)
    


    rts



_getByteEMem:
;char ... (char bank, unsignet int address)


    sta ptr1
    stx ptr1+1      ;Save address

    jsr popa        ;Get bank
    ora #$80        
    and #$8f        ;make shure that nothing hapends with other I/O devices. Disable load
    sta __PORTA__

  

    lda ptr1+1
    ora #$80
    and #$BF        ; make sure it starts at $8000 and ends at $bfff
    sta ptr1+1

    lda (ptr1)
    
    ldx #00

    rts


