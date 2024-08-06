void initACIA()
{
    asm("jsr __acia_init");

}
void putch(char c)
{
    asm("ldy #%o",c);
    asm("lda (sp),y");
    asm("jsr __aciaPrintChar");
}
char getch()
{
    char ch;
    asm("jsr __aciaGetChar");
    asm("ldy #%o",ch);
    asm("sta (sp),y");
    return ch;
    

}
void puts(char* string)
{
    int i = 0;
    while(1)
    {
        if(string[i]=='\0')
        {
            break;
        }
        putch(string[i]);
        i++;
    }

}