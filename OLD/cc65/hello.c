#include "cc65\include\string.h"
#include "cc65\include\unistd.h"
#include "cc65\include\stdlib.h"
#include "cc65\include\stdint.h"
#include "cc65\include\cc65.h"
#include "FixedMath\FixedMath.h"
#include "stdbool.h"
//#include "font8x8.h"
#include "font6x8.h"
#include "VDP.h"

#include "ps2.h"
char n = 0;
//#include "Xmodem.h"
#include "stdgraphics.c"
#include "128k.h"
#include "SetUp.h"
#include "Floppy.h"
bool userInput = true;
char carc = 0;
unsigned int b = 0;
unsigned int i = 0; //for loops with counter(for loops)
unsigned char d = 0;
uint8_t StrBuffer[32];
uint8_t InputKeyBuffer [40+1];
uint8_t InputKeyBufferPointer = 0;
uint8_t _FtPtBuffer [512];
int z = 0;
int karnelX = 0;
int karnelY = 0;
unsigned char keyStringTable[256];
uint8_t OldKeyBufferCounter = 0;
const uint8_t TknErr = 0;
const uint8_t TknDir = 1;
const uint8_t TknPwd = 2;
const uint8_t TknRoot = 3;
const uint8_t TknCd = 4;
const uint8_t TknMkDir = 5;
const uint8_t TknMkFile = 6;
const uint8_t TknFromat = 7;
const uint8_t TknCls = 8;
const uint8_t TknTstEx = 9;

char funcParametar[40];
uint16_t curBlock[5];

//make shur efloppzBuffer is emptz with 0x00
uint8_t FloppySaveSectorNum(uint16_t SectorNum){
    uint16_t side   = floppyDecodeSide(SectorNum);
    uint16_t track  = floppyDecodeTrack(SectorNum);
    uint16_t block  = floppyDecodeBlock(SectorNum);

    
    return FloppySaveSector(side,track,block);
}


void karnelPrint(uint8_t *string, bool repeatLine, bool funcPrint){
    uint8_t tmpKarnelX = karnelX;
    uint8_t tmpKarnelY = karnelY;
    OldKeyBufferCounter = KeyBufferCounter;

    
    if(karnelY >= 23){
        if(funcPrint == true){
            printf(0,23,"Scroll...");
            updateScreen();
            while(OldKeyBufferCounter == KeyBufferCounter);
            scrollScreen();
            scrollScreen();
            scrollScreen();
            scrollScreen();
            updateScreen();

            karnelY = 23-4;
            tmpKarnelY = karnelY;  
        }else{
            scrollScreen();
            karnelY = 22;
            tmpKarnelY = karnelY;
        }
    }
    if(funcPrint == false){
        printChar(0,karnelY,'>');
        karnelX = 1;
    }
    if(CurrentMode == textMode){
        while(*string != '\0') { 
        printChar(karnelX, karnelY, *string);

        if (karnelX >= 40) {
            karnelX = 0;
            karnelY ++;
        }

        string++;
        karnelX++;
    }
    }else if(CurrentMode == charGraphMode2){
        while(*string != '\0') { 
        printChar(karnelX, karnelY, *string);

        if (karnelX >= 32) {
            karnelX = 0;
            karnelY ++;
        }

        string++;
        karnelX++;
    }
    }
    printChar(karnelX, karnelY, ' ');
    if(repeatLine == true){
        karnelX = tmpKarnelX;
        karnelY = tmpKarnelY;
    }else{
        karnelX = 0;
        karnelY++;
    }
    
    updateScreen();
    
}

uint8_t formatHeader(char *name, uint32_t timeStamp, uint8_t sizeBlocks, uint16_t sizeBytes, bool directory, uint8_t *HeaderBuffer){
    int i = 0;
    int xPrint = 0;
    int endCharName = 0;
    int startCharExt = 0;
    bool hadPoint = false;

    for(i = 0; i<512;i++){

        HeaderBuffer[i] = 0x00; //Clear everything
    }


    for(i=0;i<14;i++){       //name
      if(hadPoint == false && name[i] == ' ' && directory == false){

        return 1;// name error
      }

      if(name[i] == '.' && hadPoint == false && directory == false){
        endCharName = i-1;
        startCharExt = i +1;
        hadPoint = true;
      }


    }

    if(directory == true){
        endCharName = 0;
        for(i=0;i<14;i++){
            if(name[i] == ' ' || name[i] == 13){
                endCharName = i;
            }

        }
        if(endCharName == 0){
            return 3;//bad directory name no end
        }
    }

    if(name[startCharExt+3] != ' ' && directory == false){
         if(name[startCharExt+3] != 13 && directory == false){
        return 2;//no proper end
      }
      }


    if(directory == true){
        for(i=0;i<16;i++){
            if(i < endCharName){
                HeaderBuffer[i] = name[i];
            }else{
                HeaderBuffer[i] = ' ';
            }
        }
    }

    if(directory == false){
        for(i=0;i<16;i++){
            if(i <= endCharName){
                HeaderBuffer[i] = name[i];
            }else if(i < 13){
                HeaderBuffer[i] = ' ';
            }else if(i>= 13){
                HeaderBuffer[i] = name[startCharExt+i-13];
            }
        }
    }
    

        
        HeaderBuffer[16] = (timeStamp >> 24) & 0xFF;
        HeaderBuffer[17] = (timeStamp >> 16) & 0xFF;
        HeaderBuffer[18] = (timeStamp >> 8) & 0xFF;
        HeaderBuffer[19] = timeStamp & 0xFF;
        
        
        if(sizeBlocks > 128){
            return 4; //FIle too large/Folder too big
        }
         HeaderBuffer[20] = sizeBlocks;
         
        HeaderBuffer[21] = (sizeBytes >> 8) & 0xFF;
        HeaderBuffer[22] = sizeBytes & 0xFF;
        
        HeaderBuffer[23] = directory;
    return 0;
}
uint8_t alocateBlock(uint16_t *alocatedBlockNum){
    uint16_t CurBlockChk = 0;//1 and 2 are format blocks
    uint16_t i = 0;
    uint16_t n = 0;
    uint16_t curentEntry = 0;
    bool done = false;
    *alocatedBlockNum = 0;

    
    
    for(i = 0; i < 512; i++){
            _FtPtBuffer[i] = FloppyBuffer[i];
        }
    if(floppyLoadBlockState() != 0){
        return 1;
    }
    for(i = 0; i < 512; i++){
            FloppyBuffer[i] = _FtPtBuffer[i];
        }

    //printInt(30,11,FloppyBuffer[256]);
            //printInt(30,12,FloppyBuffer[257]);
            //printInt(30,13,curentEntry);

    for(i = 0; i < 128; i++){
        curentEntry = 0;

        curentEntry  |= ((FloppyBuffer[256+(i*2)]) << 8) & 0xFFFF;
        curentEntry  |= (FloppyBuffer[256+1+(i*2)]) & 0xFFFF;  

        if(i == 0){
            
            
        }
       if(curentEntry == 0){
            
            for(n = 2;n<2880;n++){
            if(floppyChekBlockFree(n) == true){
                floppyMakeBlockOccupide(n);
                CurBlockChk = n;
                n = 2900;
            }
        }
        
        FloppyBuffer[256+i*2] = (CurBlockChk >> 8) & 0xFF;
        FloppyBuffer[256+(i*2)+1] = CurBlockChk & 0xFF;
        *alocatedBlockNum = CurBlockChk;
        
        for(i = 0; i < 512; i++){
            _FtPtBuffer[i] = FloppyBuffer[i];
        }
        if(floppySaveBlockState() != 0){
        return 2;
    }
        for(i = 0; i < 512; i++){
            FloppyBuffer[i] = _FtPtBuffer[i];
        }

           return 0;


        }
        
    }
    
   
    return 255;
}




uint8_t mkFile(char *name, uint32_t timeStamp, uint8_t sizeBlocks, uint16_t sizeBytes, uint16_t directoryHeaderNum, uint16_t *alocatedBlock){
    uint8_t error = 0;
    int i = 0;
    uint16_t tmp = 0;
    //(30,15,FloppyBuffer[257]);
    
    error = alocateBlock(&tmp);
    *alocatedBlock = tmp;
     //printInt(30,16,FloppyBuffer[257]);
    if(error != 0){
        return error;
    }

    error = FloppySaveSectorNum(directoryHeaderNum);

    
    if(error != 0){
        return error+20;
    }


    if(formatHeader(name, timeStamp, sizeBlocks, sizeBytes, false, &FloppyBuffer) != 0){
        return 30;
    }

    error = FloppySaveSector(floppyDecodeSide(*alocatedBlock),floppyDecodeTrack(*alocatedBlock),floppyDecodeBlock(*alocatedBlock));

    
    

    if(error != 0){
        return error+40;
    }

   
    


    

    return 0;

}

uint8_t mkDir(char *name, uint32_t timeStamp, uint16_t directoryHeaderNum, uint16_t *alocatedBlock){

    
    uint8_t error = 0;
    int i = 0;
    uint16_t tmp = 0;
    //(30,15,FloppyBuffer[257]);
    
    error = alocateBlock(&tmp);
    *alocatedBlock = tmp;
     //printInt(30,16,FloppyBuffer[257]);
    if(error != 0){
        return error;
    }

    error = FloppySaveSectorNum(directoryHeaderNum);

    
    if(error != 0){
        return error+20;
    }


    if(formatHeader(name, timeStamp, 1, 0, true, &FloppyBuffer) != 0){
        return 30;
    }

    error = FloppySaveSector(floppyDecodeSide(*alocatedBlock),floppyDecodeTrack(*alocatedBlock),floppyDecodeBlock(*alocatedBlock));

    
    

    if(error != 0){
        return error+40;
    }

   
    


    

    return 0;

}
void resetBlockPath(){
    int i = 0;
    for(i = 0;i<5;i++){
        curBlock[i] = 0x0000;
    }
    curBlock[0] = 2;

}

uint8_t floppyFormatNew(){
    for(i = 0; i<512;i++){

        blockState[i] = 0x00;
        FloppyBuffer[i] = 0x00;
    }

    
    floppyMakeBlockOccupide(1);
    floppyMakeBlockOccupide(2);
    floppySaveBlockState();
    if(formatHeader("root ",publicunixTimestamp,1,512,true,&FloppyBuffer) != 0){
        //printChar(0,5,FloppyBuffer[0]);
        return 1;
    }
    if(FloppySaveSector(0,0,2) != 0){
        return 2;
    }

    resetBlockPath();
    return 0;
}

void printHeader(uint8_t *Header){
    int i = 0;
    bool directory = Header[23];
    uint16_t Size = 0;
    uint8_t StrPtr = 0;
    uint8_t printBuffer[40];
    Size |= (Header[21] << 8) & 0xFFFF;
    Size |= Header[22] & 0xFFFF;

    for(i = 0;i<40;i++){
        printBuffer[i] = 0x20;
    }
    printBuffer[39] = 0;

    for(i = 0;i<16;i++){
        if(Header[i] == 13){
            i = 16;
        }
        if(i == 13 && directory == false){
            printBuffer[StrPtr] ='.';
            StrPtr++;
        }
        if(Header[i] != ' ' && Header[i] != 0x00){
            printBuffer[StrPtr] = Header[i];
            StrPtr++;
        }
    }

    


        if(directory == true){
            strcpy((char *)printBuffer+16,"<DIR>");
        }    

        if(directory == false){

            for(i = 0;i<6;i++){
                StrBuffer[i] = 0x20;
            }
            StrBuffer[6] = 0;


            strcpy((char *)printBuffer+21,"SIZE:      ");
            itoa(Size,(char *)printBuffer+27,10);
        }
        
        karnelPrint(printBuffer,false,true);
        //Date
}
uint8_t floppyLoadBlockNum(uint16_t blockNum){
    uint8_t side   = floppyDecodeSide(blockNum);
    uint8_t track  = floppyDecodeTrack(blockNum);
    uint8_t block  = floppyDecodeBlock(blockNum);

    return FloppyLoadSector(side,track,block);

}
uint8_t dumpDirectory(){
    bool done = false;
    uint16_t i = 0;
    uint16_t curentEntry = 0;
    uint8_t tmp = 0;
   for(i = 0; i < 512; i++){
        _FtPtBuffer[i] = FloppyBuffer[i];
   }

    for(i = 0; i < 128; i++){
        curentEntry = 0;
        curentEntry  |= (_FtPtBuffer[256+(i*2)] << 8) & 0xFFFF;
        curentEntry  |= _FtPtBuffer[256+1+(i*2)] & 0xFFFF;  

        if(curentEntry == 0){
            return 0;
        }

        tmp = floppyLoadBlockNum(curentEntry);
        if(tmp != 0){
            return tmp;
        }
        printHeader(&FloppyBuffer);
        
        
    }
    return 0;
}


void keyBoardInputToBufferUpdate(bool *parse){
    if(__CursorState == true && KeyBufferCounter == OldKeyBufferCounter && *parse == false){
            InputKeyBuffer[InputKeyBufferPointer] = 0xDC;
        }else{
            InputKeyBuffer[InputKeyBufferPointer] = 0x20;
        }

    if(*parse == true){
        return;
    }
    if(InputKeyBufferPointer > 39){
        InputKeyBuffer[39] = 0x20;
        InputKeyBuffer[40] = 0x00;
        InputKeyBufferPointer--;
        return;
    }
    if(KeyBufferCounter != OldKeyBufferCounter){
        if(KeyBuffer[KeyBufferCounter] == 0x08){
            OldKeyBufferCounter = KeyBufferCounter;
            if(InputKeyBufferPointer != 0){
                InputKeyBufferPointer -= 1;
            }
            
            InputKeyBuffer[InputKeyBufferPointer] = ' ';
            return;
        }

        if(KeyBuffer[KeyBufferCounter] == 0x0D){
            OldKeyBufferCounter = KeyBufferCounter;
            InputKeyBuffer[InputKeyBufferPointer] = 0x00;
            *parse = true;
            return;
        }


        OldKeyBufferCounter = KeyBufferCounter;
        InputKeyBuffer[InputKeyBufferPointer] = KeyBuffer[KeyBufferCounter];
        InputKeyBufferPointer++;
    }

}




void UpdateParse(bool *parse, uint8_t *curToken){
    char *ptr;
    char delim[] = " ";
    int i = 0;
    for(i=0;i<40;i++){

        funcParametar[i] = 0;
    }


    if(*parse == false){
        return;
    }
    *parse = false;
    ptr = strtok(&InputKeyBuffer,delim);
    //printf(26,20,ptr);

    if(strcmp(ptr,"dir") == 0){
        *curToken = TknDir;
    }else if(strcmp(ptr,"pwd") == 0){
        *curToken = TknPwd;
    }else if(strcmp(ptr,"root") == 0){
        *curToken = TknRoot;
    }else if(strcmp(ptr,"cd") == 0){
        *curToken = TknCd;
        ptr = strtok(NULL,delim);
        strcpy(funcParametar,ptr);
        
    }else if(strcmp(ptr,"mkdir") == 0){
        *curToken = TknMkDir;
        ptr = strtok(NULL,delim);
        strcpy(funcParametar,ptr);
    }else if(strcmp(ptr,"mkfile") == 0){
        *curToken = TknMkFile;
        ptr = strtok(NULL,delim);
        strcpy(funcParametar,ptr);
    }else if(strcmp(ptr,"formatYes") == 0){
        *curToken = TknFromat;
    }else if(strcmp(ptr,"cls") == 0){
        *curToken = TknCls;
    }else if(strcmp(ptr,"testex") == 0){
        *curToken = TknTstEx;
    }



//end of parse
    while(ptr != NULL){
        ptr = strtok(NULL,delim);
    }

    for(i = 0;i<40;i++){
        InputKeyBuffer[i] = 0x20;
    }
    InputKeyBufferPointer = 0;
}






void openRoot(){
    resetBlockPath();
    floppyLoadBlockNum(2);
}

uint16_t lastDirectory(){       //returns block of header of last directory
    uint16_t i = 0;
    while(curBlock[i] != 0){
        i++;
    }
    i--;
    return curBlock[i];
}
void openLastDirectory(){
    floppyLoadBlockNum(lastDirectory());

}


void  printPath(){
    int i = 0;
    int n = 0;
    int offset = 0;
    char pathBuffer[16*5+1];

    for(i = 0;i<16*5+1;i++){
        pathBuffer[i] = 0;
    }

    for(i = 0;i<5;i++){
        if(curBlock[i] == 0){
            karnelPrint(&pathBuffer,false,true);
            return;
        }
        floppyLoadBlockNum(curBlock[i]);

        
        for(n = 0; FloppyBuffer[n] != 0 && FloppyBuffer[n] != ' ';n++){
            pathBuffer[offset+n] = FloppyBuffer[n];
        }
        offset = n+offset;
        pathBuffer[offset] = 0x5C; // "\"
        offset++;
    }
    //pathBuffer[0] = "tst khm khm";
    
    
}

uint8_t findEntry(uint8_t *name){
    int i = 0;
    int n = 0;
    int namePtr = 0;
    uint16_t curentEntry = 0;
    uint16_t previousEntry = 0;
    openLastDirectory();

    for(i = 0; i < 128; i++){
        curentEntry = 0;
        curentEntry  |= (FloppyBuffer[256+(i*2)] << 8) & 0xFFFF;
        curentEntry  |= FloppyBuffer[256+1+(i*2)] & 0xFFFF;  

        if(curentEntry == 0){
            karnelPrint((uint8_t*)"Entry not found",false,true);
            return 0;
        }
        for(n = 0; n < 512; n++){   //save buffer
            _FtPtBuffer[n] = FloppyBuffer[n];
        }
        if(floppyLoadBlockNum(curentEntry) != 0){
            karnelPrint(&"Error loading fromn floppy!",false,true);
            return 0;
        }

    
        

        
        
        
        for(n = 0; n < 32; n++){   //save buffer
            StrBuffer[n] = 0x00;
        }

        namePtr = 0;
        for(n = 0;n<16;n++){
        if(FloppyBuffer[n] == 13){
            n = 16;
        }
        if(n == 13 && FloppyBuffer[n] != ' '){
            StrBuffer[namePtr] = '.';
            namePtr++;
        }
        if(FloppyBuffer[n] != ' '){
            StrBuffer[namePtr] = FloppyBuffer[n];
            namePtr++;
        }
        
    }
        
        if(strcmp((char*)StrBuffer,(char*)name) == 0){
            //karnelPrint(name,false,true);
            return curentEntry;
        }
        for(n = 0; n < 512; n++){   //save buffer
            FloppyBuffer[n] = _FtPtBuffer[n];
        }

    }

    

}

void findOpenDirectory(uint8_t *name){
    uint16_t entry = 0;
    int i = 0;

    if(strcmp((char *)name,"..") == 0){
        while(curBlock[i] != 0){
        i++;
    }
    i--;
    if(i == 0){
        return;
    }
    curBlock[i] = 0x00;   //go back so delet last entry

        return;
    }
    if(strcmp((char *)name,".") == 0){
        return;
    }


    entry = findEntry(name);

    if(entry == 0){
        return;
    }
    
    
    if(FloppyBuffer[23] != true){
        karnelPrint((uint8_t *)"Not a directory",false,true);
    }
    while(curBlock[i] != 0){
        i++;
    }
    curBlock[i] = entry;


}

void makeDirectory(uint8_t *name){
    uint16_t curDir = lastDirectory();
    uint16_t alocatedBlock = 0;
    int i = 0;
    strcat((char *)name, &" ");
    openLastDirectory();
    if(mkDir((char *)name, publicunixTimestamp, curDir, &alocatedBlock) != 0){
        karnelPrint((uint8_t *)"Error making directory",false,true);
        return;
    }
    openLastDirectory();
}

void makeFile(uint8_t *name){
    uint16_t curDir = lastDirectory();
    uint16_t alocatedBlock = 0;
    int i = 0;
    strcat((char *)name, &" ");
    openLastDirectory();
    if(mkFile((char *)name, publicunixTimestamp,1,0, curDir, &alocatedBlock) != 0){
        karnelPrint((uint8_t *)"Error making file",false,true);
        return;
    }
    openLastDirectory();
}




void testExMem(){
    int bank = 0;
    uint16_t address = 0;
    uint8_t byteNum = 0;
    int i = 0;
    uint16_t errors = 0;
    

    for(bank = 0;bank<16;bank++){
        for(i=0;i<40;i++){
        StrBuffer[i] = 0x20;
    }
    StrBuffer[39] = 0x00;

        errors = 0;
        strcpy((char *)StrBuffer,"Testing bank            ");
        itoa(bank,(char *)StrBuffer+13,10);
        karnelPrint(StrBuffer,false,true);
        karnelY--;
        
        for(address = 0;address<0x4000;address++){

                byteNum = 255;
                putByteEMem(byteNum, bank, address);
                if(getByteEMem(bank, address) != byteNum){
                    errors ++;
                }
                byteNum = 0;
                putByteEMem(byteNum, bank, address);
                if(getByteEMem(bank, address) != byteNum){
                    errors ++;
                }
                
            
        }    

        if(errors != 0){
            strcpy((char *)StrBuffer+20,"FAIL   ");
            ltoa(errors,(char *)StrBuffer+26,10);
        }else{
            strcpy((char *)StrBuffer+20,"PASS   ");
        }
        for(i=0;i<20;i++){
        if(StrBuffer[i] == 0x00){
            StrBuffer[i] = 0x20;
        }
    }
        karnelPrint(StrBuffer,false,true);
    }
}
void doFunction(uint8_t *curToken){
    int i = 0;
    


    if(*curToken == TknDir){
        openLastDirectory();
        dumpDirectory();
    }else if(*curToken == TknRoot){
        openRoot();
    }if(*curToken == TknPwd){
        printPath();
    }if(*curToken == TknCd){
        findOpenDirectory(&funcParametar);
    }if(*curToken == TknMkDir){
        makeDirectory(&funcParametar);
    }if(*curToken == TknMkFile){
        makeFile(&funcParametar);
    }if(*curToken == TknFromat){
        floppyFormatNew();
        karnelPrint((uint8_t *)"Format completed...",false,true);
        karnelPrint((uint8_t *)"All data is deleted!",false,true);
    }if(*curToken == TknCls){
        karnelY = 0;
        for(i = 0;i<960;i++){
            VbufferPublic[i] = 0x20;
        }
        updateScreen();
    }if(*curToken == TknTstEx){
       testExMem();
    }


    *curToken = 0;
}





//MAIN
void main(void){
    int i = 0;
    int y = 2;
    uint16_t minute = 56;
    uint16_t hour = 22;
    uint16_t day = 27;
    uint16_t month = 2;
    uint16_t year = 2023;
    uint32_t unixTimestamp = dateToTimestamp(minute, hour, day, month, year);
    uint16_t alocatedBlock = 0;
    uint8_t curToken = 0;
    uint8_t TestBuffer[] = "alo???";
    bool parse = false;
    OldKeyBufferCounter = KeyBufferCounter;
    InputKeyBufferPointer = 0;
    karnelX = 0;
    karnelY = 0;
    
    initIO();
    
    publicunixTimestamp = dateToTimestamp(20,21,5,7,2023);
    
     FloppyMotorOn();
    //Format Floppy
    //floppyFormatNew();


    for(i = 0;i<40+1;i++){
        InputKeyBuffer[i] = 0;
    }

    //updateScreen();
    

    //printInt(0,10,mkFile("rudjer.ers ",publicunixTimestamp,1,100,2, &alocatedBlock));
    //printInt(0,11,alocatedBlock);
    //FloppyLoadSector(0,0,2);
    //printInt(3,10,mkFile("tata.ers ",publicunixTimestamp,1,150,2, &alocatedBlock));
    //printInt(3,11,alocatedBlock);
    //FloppyLoadSector(0,0,2);
    //printInt(6,10,mkFile("HelloWorld.ers ",publicunixTimestamp,1,168,2, &alocatedBlock));
    //printInt(6,11,alocatedBlock);
    //printInt(26,0,alocatedBlock);
    //FloppyLoadSector(0,0,2);
    //printHeader(&y, &FloppyBuffer);
    //updateScreen();
    //FloppyLoadSector(floppyDecodeSide(alocatedBlock),floppyDecodeTrack(alocatedBlock),floppyDecodeBlock(alocatedBlock));
    //FloppyLoadSector(0,0,2);
    //printHeader(&y, &FloppyBuffer);

    //FloppyLoadSector(0,0,3);
    //printHeader(&y, &FloppyBuffer);
    //FloppyLoadSector(0,0,4);
    //printHeader(&y, &FloppyBuffer);
    //FloppyLoadSector(0,0,5);
    //printHeader(&y, &FloppyBuffer);
   //dumpDirectory(&FloppyBuffer);
    resetBlockPath();
    while(true){

        timestampToDate(publicunixTimestamp, &year, &month, &day, &hour, &minute);
        keyBoardInputToBufferUpdate(&parse);
        if(parse == true){
            userInput = false;
        }
        
        
       
        

        karnelPrint(&InputKeyBuffer, userInput, false);
        UpdateParse(&parse,&curToken);
        doFunction(&curToken);
        updateScreen();
        updateTimeStamp();
        
        userInput = true; // reset to user input
    }
    
    while(true);






}




