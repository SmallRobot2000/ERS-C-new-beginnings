#include<ACIA.h>

void puts(char* str)
{
    int i = 0;
    while(1)
    {
        if(str[i] == '\0') break;
        putch(str[i]);
        i++;
    }

    
}