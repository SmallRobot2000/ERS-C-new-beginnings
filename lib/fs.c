#include<fs.h>
#include<SDSPI.h>
#include<ACIA.h>
struct superBlockStr superBlock;

uint8_t fs_Init()
{
    superBlock.Magic = MAGIC_NUMBER;
    superBlock.Blocks = NUMBER_BLOCKS;
    superBlock.EntryBlocks = ENTRY_BLOCKS;
    superBlock.EntrysPerBlock = ENTRYS_PER_BLOCK;

    return 0;
}
uint8_t SD_writeBlock(uint32_t blockNum, void* buffer)
{
    uint8_t res = 0;
    int i = 0;
    res |= SDSPI_writeMultStart(blockNum*SECTORS_IN_BLOCK);
    for(i = 0; i < SECTORS_IN_BLOCK;i++)
    {
        res |= SDSPI_writeMultContinue((uint8_t *)(buffer)+i*512);
    }
    
    res |= SDSPI_writeMultEnd();
    return res;
    
}
uint8_t SD_readBlock(uint32_t blockNum, void* buffer)
{
    uint8_t res = 0;
    int i = 0;
    res |= SDSPI_readMultStart(blockNum*SECTORS_IN_BLOCK);
    if(res != 0)
    {
        puts("1");
    }
    for(i = 0; i < SECTORS_IN_BLOCK;i++)
    {
        res |= SDSPI_readMultContinue((uint8_t *)(buffer)+i*512);
        if(res != 0)
    {
        puts("2");
    }
    }
    
    res |= SDSPI_readMultEnd();
    if(res != 0)
    {
        puts("3");
    }
    return res;
    
}
uint8_t fs_format()
{
    return 0;
}
void printHex32(uint32_t n)
{
    print_hex(n>>24);
    print_hex(n>>16);
    print_hex(n>>8);
    print_hex(n);
    
}

