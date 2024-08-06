uint32_t  IRQCounter = 0;
bool publicVDPIe = true;

extern void __fastcall__ vdp_debug();
extern void __fastcall__ VDP_W_REG(unsigned char reg, unsigned char val);
extern unsigned char __fastcall__ VDP_R_STAT();
extern void __fastcall__ VDP_W_VRAM(unsigned int addr, unsigned char val);
extern void __fastcall__ VDP_W_BLOCK_asm(void *ptr_ram, unsigned int ptr_Vram, unsigned int len);

//C functions



void VDP_W_BLOCK(void *ptr_ram, unsigned int ptr_Vram, unsigned int len){

    VDP_W_BLOCK_asm(ptr_ram, ptr_Vram, len);

}


#include "VDP.c"        




