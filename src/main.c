#include<ACIA.h>
#include<SDSPI.h>
#include<fs.h>
#include<stdint.h>
#include<stdlib.h>
uint8_t buffer[512];
uint8_t buffer1[512];

void foo()
{
}

int main() 
{ 
    char ch;
    int i;
    int n;
    foo();
    putch('F');
    SDSPI_initSD();
    ch = 0;
    
    printHex32(0x11223345);
    i = 0;
    putch('F');
    n = 0;
    //while(1);
    
    
    putch('F');
    putch('F');
    putch('F');
    putch('F');
    puts("\n\rTest ");
    buffer[510] = 0xCA;
    buffer[511] = 0x4F;
    
    printHex32(0x11223345);
    puts("\n\rTest ");

    if(SDSPI_writeMultStart(0x555) != 0)
    {
        puts("\r\nno");
        return 0;
    }
    for(i = 0;i < 25;i++)
    {
        SDSPI_writeMultContinue(buffer);
    }
    SDSPI_writeMultContinue(buffer);
    SDSPI_writeMultEnd();

    for(i = 0;i < 25;i++)
    {
        SDSPI_readSector(0x555+i,buffer1);
        for(n = 0;n < 25; n++)
        {
            if(buffer[n] != buffer1[n])
            {
                puts("\n\rFailed at sector ");
                printHex32(i+0x555);
                puts("\n\r");
            }
        }
    }
    puts("Alive?");

    
    SDSPI_readSector(0x555,buffer);
    print_hex(buffer[510]);
    print_hex(buffer[511]);
    initACIA();
    puts("Alo");
    




    while(1)
    {
        ch = getch();
        if(ch == 3)
        {
            break;
        }
        putch(ch);
    }
    return 0; 
}

