echo off

echo Building hello.c
cc65 -W-pointer-types -t none -O --cpu 65sc02 hello.c

echo Assembling...
ca65 --cpu 65sc02 hello.s
ca65 --cpu 65sc02 interrupt.s
ca65 --cpu 65sc02 vectors.s
ca65 --cpu 65sc02 wait.s
ca65 --cpu 65sc02 lcd.s
ca65 --cpu 65sc02 VDP.s
ca65 --cpu 65sc02 ps2.s
ca65 --cpu 65sc02 FixforFixMath.s
REM ca65 --cpu 65sc02 Xmodem.s
ca65 --cpu 65sc02 128k.s
ca65 --cpu 65sc02 floppy.s
echo Linking...
REM no vectors.o or interupt.o or Xmodem.o lcd.o
ld65 -C breadboard.cfg -m hello.map wait.o  ps2.o floppy.o hello.o FixforFixMath.o 128k.o VDP.o none.lib -o PROGRAM.out FixedMath.lib 

echo Copying to SD... Radiona
copy PROGRAM.out E:\SUBFOLDR
echo Copying to SD... Glavni
copy PROGRAM.out I:\SUBFOLDR
echo Done.
