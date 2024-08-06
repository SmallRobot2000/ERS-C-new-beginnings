#include<stdint.h>

#define SECTORS_IN_BLOCK 8
#define MAGIC_NUMBER 0xA0A055FF
#define NUMBER_BLOCKS 0x100000
#define ENTRY_BLOCKS 0x800
#define ENTRYS_PER_BLOCK 0x80
#define DATA_USE_BLOCKS 0x20
#define SPUER_BLOCK 0x0000
#define ENTRY_START_BLOCK SPUER_BLOCK+1
#define DATA_USE_BLOCKS_START ENTRY_START_BLOCK+ENTRY_BLOCKS
#define DATA_BLOCKS_START DATA_USE_BLOCKS_START+DATA_USE_BLOCKS
#define DATA_BLOCKS NUMBER_BLOCKS-DATA_BLOCKS_START
struct superBlockStr
{
    uint32_t Magic;
    uint32_t Blocks;
    char     DriveName[23];
    uint32_t EntryBlocks;
    uint8_t  EntrysPerBlock;
    uint32_t SuperBlockPtr;
    uint32_t EntryStartBlockPtr;
    uint32_t UsedDataStartBlockPtr;
    uint32_t DataBlockPtr;
};

uint8_t fs_Init();
uint8_t fs_format();
uint8_t SD_writeBlock(uint32_t blockNum, void* buffer);
uint8_t SD_readBlock(uint32_t blockNum, void* buffer);
void printHex32(uint32_t n);
