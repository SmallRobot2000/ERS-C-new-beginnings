#include<stdint.h>
extern void __fastcall__ print_hex(uint8_t uint);
extern uint8_t __fastcall__ SDSPI_readSector(uint32_t sector, uint8_t *buffer);
extern uint8_t __fastcall__ SDSPI_writeSector(uint32_t sector, uint8_t *buffer);
extern uint8_t __fastcall__  SDSPI_initSD();
extern uint8_t __fastcall__ SDSPI_readMultStart(uint32_t sectorStart);
extern uint8_t __fastcall__ SDSPI_readMultContinue(uint8_t *buffer);
extern uint8_t __fastcall__ SDSPI_readMultEnd();
extern uint8_t __fastcall__ SDSPI_writeMultStart(uint32_t sectorStart);
extern uint16_t __fastcall__ SDSPI_writeMultContinue(uint8_t *buffer);
extern uint8_t __fastcall__ SDSPI_writeMultEnd();



