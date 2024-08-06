echo off

echo Building main.c
cc65 -t none -O --cpu 65sc02 src/main.c -o bin/main.asm -I include
cc65 -t none -O --cpu 65sc02 lib/mylibC.c -o bin/mylibC.asm -I include
rem cc6502 -W-pointer-types -t none -O --cpu 65sc02 src/include/ACIA.c -o bin/ACIA.asm
rem REM fat32
rem cc6502 -W-pointer-types -t none -O --cpu 65sc02 fat32/src/diskio.c -o bin/fat32/diskio.o
rem cc6502 -W-pointer-types -t none -O --cpu 65sc02 fat32/src/ff.c -o bin/fat32/ff.o
rem cc65 -W-pointer-types -t none -O --cpu 65sc02 fat32/src/ffsystem.c -o bin/fat32/ffsystem.o
rem cc65 -W-pointer-types -t none -O --cpu 65sc02 fat32/src/ffunicode.c -o bin/fat32/ffunicode.o









echo Assembling...
ca65 --cpu 65sc02 bin/main.asm -o bin/main.o
ca65 --cpu 65sc02 lib/crt0.asm -o bin/crt0.o
ca65 --cpu 65sc02 lib/mylib.asm -o bin/mylib.o
ca65 --cpu 65sc02 bin/mylibC.asm -o bin/mylibC.o
ca65 --cpu 65sc02 lib/SDSPI.asm -o bin/SDSPI.o
rem ca6502 --cpu 65sc02 cfg/startup.asm -o bin/startup.o
rem ca6502 --cpu 65sc02 lib/mylib.asm -o bin/mylib.o
rem ca6502 --cpu 65sc02 bin/ACIA.asm -o bin/ACIA.o


REM fat32











REM ca65 --cpu 65sc02 interrupt.s
REM ca65 --cpu 65sc02 vectors.s
REM ca65 --cpu 65sc02 wait.s
REM ca65 --cpu 65sc02 lcd.s
REM ca65 --cpu 65sc02 VDP.s
REM ca65 --cpu 65sc02 ps2.s
REM ca65 --cpu 65sc02 FixforFixMath.s
REM REM ca65 --cpu 65sc02 Xmodem.s
REM ca65 --cpu 65sc02 128k.s
REM ca65 --cpu 65sc02 floppy.s
echo Linking...
REM no vectors.o or interupt.o or Xmodem.o lcd.o
ld65 -C cfg/ERS.cfg bin/main.o bin/crt0.o bin/mylib.o bin/mylibC.o bin/SDSPI.o lib/none.lib -o bin/PROGRAM.out 
fileFormater.exe bin/PROGRAM.out bin/PROGRAM.ers
REM echo Copying to SD... Radiona
REM copy PROGRAM.out E:\SUBFOLDR
REM echo Copying to SD... Glavni
REM copy PROGRAM.out I:\SUBFOLDR
echo Done.
