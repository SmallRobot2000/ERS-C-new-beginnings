
unsigned char VbufferPublic[6144];
unsigned char CurrentMode = 0;
unsigned int VDPnameTableAddress          =   0x1800;
unsigned int VDPspriteAttributeAddress    =   0x1C00;
unsigned int VDPpaterGeneratorAddress     =   0x2000;
unsigned int VDPcolorTableAddress         =   0x0000;
unsigned int VDPspriteGeneratorAddress    =   0x3800;

const unsigned char VDPreg2AddNum   =   0x06;
const unsigned char VDPreg3AddNum   =   0x7f;
const unsigned char VDPreg4AddNum   =   0x07;
const unsigned char VDPreg5AddNum   =   0x38;
const unsigned char VDPreg6AddNum   =   0x07;

const unsigned char transperent     =   0;
const unsigned char black           =   1;
const unsigned char mediumGreen     =   2;
const unsigned char lightGreen      =   3;
const unsigned char darkBlue        =   4;
const unsigned char lightBlue       =   5;
const unsigned char darkRed         =   6;
const unsigned char cyan            =   7;
const unsigned char mediumRed       =   8;
const unsigned char lightRed        =   9;
const unsigned char darkYellow      =   10;
const unsigned char lightYellow     =   11;
const unsigned char darkGreen       =   12;
const unsigned char magenta         =   13;
const unsigned char gray            =   14;
const unsigned char white           =   15;

const unsigned charGraphMode1    = 0;
const unsigned charGraphMode2    = 1;
const unsigned multicolorMode    = 2;
const unsigned textMode          = 3;

uint8_t SCREEN_WIDTH_CHAR = 0;
void initVDP(bool enable, bool memSize, bool interupt, bool spriteSize, bool spriteMag, unsigned char mode){
/*
mode    M1  M2  M3      name
0       0   0   0       Graphics mode 1
1       0   0   1       Graphics mode 2
2       0   1   0       Multicolor mode
3       1   0   0       Text mode
*/
 

unsigned char regData = 0;
CurrentMode = mode;

if(mode == 1){
    SCREEN_WIDTH_CHAR = 32;
    VDPpaterGeneratorAddress     =   0x2000;
    VDP_W_REG(0, 0x02);     //set Reg 0 to %0000 0010   

}

regData = 0;
if(memSize == true){

    regData = regData|0x80;

  }

if (enable == true){

    regData = regData|0x40;
}

if (interupt == true){
    __asm__ ("cli");
    regData = regData|0x20;
}else{

    //__asm__ ("cli");

    regData = regData&0xDF;
}

if (mode == 3){
    SCREEN_WIDTH_CHAR = 40;
    regData = regData|0x10;
    VDPpaterGeneratorAddress     =   0x3800;
}

if (mode == 2){

    regData = regData|0x08;
}

if (spriteSize == true){

    regData = regData|0x02;
}

if (spriteMag == true){

    regData = regData|0x01;
}

 VDP_W_REG(1, regData);     //set Reg 1 to defined bool's  


 VDP_W_REG(2, VDPreg2AddNum);
 VDP_W_REG(3, VDPreg3AddNum);
 VDP_W_REG(4, VDPreg4AddNum);
 VDP_W_REG(5, VDPreg5AddNum);
 VDP_W_REG(6, VDPreg6AddNum);   //set tables
}

void colorGraphicsVDP(unsigned char TextColor, unsigned char BackGround){
    int i = 0;
    __asm__("sei");

     for(i = 0; i < 6144; i++){
        VbufferPublic[i] = ((TextColor << 4)|BackGround);
}
    VDP_W_BLOCK(VbufferPublic, VDPcolorTableAddress, 6143);



     __asm__("cli");



}
void colorBorderVDP(unsigned char TextColor, unsigned char BackGround){
    int i = 0;
    __asm__("sei");
    TextColor = TextColor << 4;
    BackGround = BackGround|TextColor;
    VDP_W_REG(7, BackGround);
    __asm__("sei");

}

void printChar(unsigned char x, unsigned char y, unsigned char a){
    int i = 0;
    unsigned int add = 0;
    __asm__("sei");
    
    if(CurrentMode == charGraphMode2){
        add = x*8+y*256;
        __asm__("sei");
        for(i = 0;i != 8;i++){
            __asm__("sei");
            VbufferPublic[add+i] = font[a*8+i];

        }

    }else if(CurrentMode == textMode){
        add = x+y*40;
        VbufferPublic[add] = a;

    }


    __asm__("cli");
}





void printf(unsigned char x, unsigned char y, const char *str) {
    int b = 0;
    __asm__("sei");
    if(CurrentMode == charGraphMode2){
    while(*str != '\0') { 
        printChar(x, y, *str);

        if (x >= 32) {
            x = 0;
            y ++;
        }

        str++;
        x++;
    }
    }else if(CurrentMode == textMode){
    while(*str != '\0') { 
        printChar(x, y, *str);

        if (x >= 40) {
            x = 0;
            y ++;
        }

        str++;
        x++;
    }
    }
    __asm__("cli");
}


void scrollScreen(){
    int i = 0;
    if(CurrentMode == charGraphMode2){
        for(i = 0;i<6144;i++){
            VbufferPublic[i] = VbufferPublic[i+256];
        }
        for(i = 0;i<32;i++){
            printChar(i,23,' ');
        }
    }

    if(CurrentMode == textMode){
        for(i = 0;i<960;i++){
            VbufferPublic[i] = VbufferPublic[i+40];
        }
        for(i = 0;i<40;i++){
            printChar(i,23,' ');
        }
    }
}


void updateScreen(){
  __asm__("sei");
  if(CurrentMode == charGraphMode2){
    VDP_W_BLOCK(VbufferPublic, VDPpaterGeneratorAddress, 6144); // update screen
  }

  if(CurrentMode == textMode){
    VDP_W_BLOCK(VbufferPublic, VDPnameTableAddress, 960); // update screen
  }

}

