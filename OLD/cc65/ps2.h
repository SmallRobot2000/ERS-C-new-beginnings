const unsigned char shiftCodeLeft = 0x12;
const unsigned char shiftCodeRight = 0x59;
const unsigned char releseCode = 0xf0;

const unsigned int ACIA_data    = 0x0460;
const unsigned int ACIA_stat    = 0x0461;
const unsigned int ACIA_cmd     = 0x0462;
const unsigned int ACIA_ctrl    = 0x0463;

bool shifted = false;
bool relesed = false;

extern void __fastcall__ setIrqErs();
extern void __fastcall__ cli();
extern unsigned char __fastcall__ getKeyCode();

extern unsigned char __fastcall__ getKeyMap(unsigned char code);
extern unsigned char __fastcall__ getKeyMapShifted(unsigned char code);

extern unsigned char __fastcall__ peek(unsigned int address);

extern void __fastcall__ poke(unsigned int address, unsigned char data);



extern void __fastcall__ ACIA_asm_init(unsigned int das);

void ACIAinit(unsigned int Baud){
        unsigned char ctrl = 16; //Baud rate enabled
        char new_Baud;
        if(Baud == 1200){

        }else if (Baud == 1800){
                new_Baud = 0x09;
        }else if (Baud == 3600){
                new_Baud = 0x0b;
        }else if (Baud == 4800){
                new_Baud = 0x0c;
        }else if (Baud == 7200){
                new_Baud = 0x0d;
        }else if (Baud == 9600){
                new_Baud = 0x0e;
        }else if (Baud == 19200){
                new_Baud = 0x0f;
        }
        ctrl = ctrl|new_Baud;
        //ACIA_asm_init(ctrl);
        }

unsigned char KeyBufferCounter = 0;
char KeyBuffer[256];

