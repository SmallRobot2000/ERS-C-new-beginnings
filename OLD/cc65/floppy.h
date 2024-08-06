uint8_t FloppyBuffer[512];
uint8_t FloppyBuffer1[512];
bool blockState[360];

extern char __fastcall__ FloppyTest();
extern void __fastcall__ FloppySetReceveSerial();
extern void __fastcall__ FloppySetSendSerial();
extern void __fastcall__ FloppySendSerial(char data);
extern char __fastcall__ FloppyReceveSerial();
extern void __fastcall__ waite();

void FloppySendBlock(){

    int i = 0;
    int z = 0;
    unsigned char chekSum = 0;
    FloppySetSendSerial();
    waite();
    FloppySendSerial(0x69);
    while(z < 512){

      chekSum += FloppyBuffer[z];
      z++;
        
    } 

    i = 0;
    while(i < 512){

        FloppySendSerial(FloppyBuffer[i]);
        i++;
    } 
        waite();
        FloppySendSerial(chekSum);
}


void FloppyReceveBlock() {

  int z = 0;
  unsigned char chekSum = 0;
  unsigned char chekSum1= 0;
  unsigned char chekSumGot = 0;
  unsigned char wait = 0;
  int x = 0;
  int i = 0;
  FloppySetReceveSerial();

  
  while(wait != 0x95){
    FloppySetSendSerial();
    FloppySendSerial(0x90);
    waite();
    waite();
    FloppySetReceveSerial();
    waite();
    waite();
    wait = FloppyReceveSerial();

}
   
    FloppySetReceveSerial();
    waite();

    FloppyBuffer[0 ] = FloppyReceveSerial();
    z = 0;
  while (z < 512) {
  
    
     
    
    FloppyBuffer[z] = FloppyReceveSerial();
    FloppyBuffer1[z] = FloppyBuffer[z];
    
    
    //printInt(0,y+4,FloppyBuffer[z]);
    //VDP_W_BLOCK(VbufferPublic, VDPpaterGeneratorAddress, 6144); // update screen
    z++;
  }

  //Chek sum
  waite();
  chekSumGot = FloppyReceveSerial();
  

  z = 0;
  chekSum1 = 0;
  
  while (z != 512) {

    chekSum1 += FloppyBuffer[z];
    z++;
  }

  //printInt(10,10,chekSum1);
  //printInt(10,11,chekSumGot);

  if (chekSum1 != chekSumGot) {
    //printf(0,y,"Cheksum error");
    //printf(0,y+1,"Cheksum is : ");
    //printInt(0,y+2,chekSum1);
    //printf(0,y+3,"Cheksum got is : ");
    //printInt(0,y+4,chekSumGot);
  } else {
    //printf(0,y,"Block receved correctly!");
  }
  //VDP_W_BLOCK(VbufferPublic, VDPpaterGeneratorAddress, 6144); // update screen
}

void FloppySendCommand(char cmmNum, char parameter0, char parameter1, char parameter2){
   char wait = 0;
   __asm__("sei");
  FloppySetSendSerial();


while(wait != 0x89){
  FloppySetSendSerial();
  waite();
  FloppySendSerial(0x79);
  waite();
  FloppySetReceveSerial();
  waite();
  wait = FloppyReceveSerial();
  waite();
}


  FloppySetSendSerial();
  waite();
  FloppySendSerial(cmmNum);
  waite();
  FloppySendSerial(parameter0);
  waite();
  FloppySendSerial(parameter1);
  waite();
  FloppySendSerial(parameter2);


}



uint8_t FloppySaveSector(char side, char track, char sector){ //side 0/1 track 0-79 sector 1-18
    unsigned char test = 0;
    unsigned char wait = 0;
     __asm__("sei");
    waite();
    waite();
    waite();
    waite();
    FloppySendCommand(0x80, side, track, sector);

   
    FloppySendBlock();
    FloppySetReceveSerial();
    waite();
    waite();
    


    while(wait != 0x95){
  FloppySetSendSerial();
  FloppySendSerial(0x90);
  waite();
  waite();
  FloppySetReceveSerial();
  waite();
  waite();
  wait = FloppyReceveSerial();

}



      test = FloppyReceveSerial();

  __asm__("cli");
      if(test != 0){
      //printf(0,0,"Error during saving block. Error code:");
      //printInt(0,1,test);
      //VDP_W_BLOCK(VbufferPublic, VDPpaterGeneratorAddress, 6144); // update screen

        return test;
      }
    return test;
    //printf(0,0,"Block saved no errors");
        //VDP_W_BLOCK(VbufferPublic, VDPpaterGeneratorAddress, 6144); // update screen


}


uint8_t FloppyLoadSector(char side, char track, char sector){
    unsigned char test = 0;
    unsigned char wait = 0;
    waite();
    waite();
    waite();
    waite();
   
    FloppySendCommand(0x81, side, track, sector);

    FloppyReceveBlock();
    FloppySetReceveSerial();
    waite();
    waite();

    

while(wait != 0xb5){
  FloppySetSendSerial();
  FloppySendSerial(0xb0);
  waite();
  waite();
  FloppySetReceveSerial();
  waite();
  waite();
  wait = FloppyReceveSerial();
}




      test = FloppyReceveSerial();


  __asm__("cli");    
        return test;
      




}






void FloppyMotorOn(){
    unsigned char test = 0;
    unsigned char wait = 0;
 
    FloppySendCommand(0x82, 0, 0, 0);

    waite();
    waite();
    waite();
    waite();
    waite();
    waite();


    

while(wait != 0xb5){
  FloppySetSendSerial();
  waite();
  FloppySendSerial(0xb0);
  waite();
  FloppySetReceveSerial();
  waite();
  wait = FloppyReceveSerial();
  waite();
}



      waite();
      test = FloppyReceveSerial();
      waite();

      if(test != 0){
      printf(0,0,"Error");
      printInt(0,8,test);
      VDP_W_BLOCK(VbufferPublic, VDPpaterGeneratorAddress, 6144); // update screen
      }
  __asm__("cli");

}




bool FloppyHaveDisk(){
    unsigned char test = 0;
    unsigned char wait = 0;
    FloppySendCommand(0x83,0,0,0);

    while(wait != 0xb5){
      FloppySetSendSerial();
      waite();
      FloppySendSerial(0xb0);
      waite();
      FloppySetReceveSerial();
      waite();
      wait = FloppyReceveSerial();
      waite();

  }


      waite();
      test = FloppyReceveSerial();
      waite();
      __asm__("cli");

      return test;

}


void FloppyMotorOff(){
    unsigned char test = 0;
    unsigned char wait = 0;
    FloppySendCommand(0x84,0,0,0);

    while(wait != 0xb5){
      FloppySetSendSerial();
      waite();
      FloppySendSerial(0xb0);
      waite();
      FloppySetReceveSerial();
      waite();
      wait = FloppyReceveSerial();
      waite();

  }


      waite();
      test = FloppyReceveSerial();
      waite();

  __asm__("cli");

}



bool FloppyMotorRunning(){
    unsigned char test = 0;
    unsigned char wait = 0;
    FloppySendCommand(0x85,0,0,0);

    while(wait != 0xb5){
      FloppySetSendSerial();
      waite();
      FloppySendSerial(0xb0);
      waite();
      FloppySetReceveSerial();
      waite();
      wait = FloppyReceveSerial();
      waite();

  }


      waite();
      test = FloppyReceveSerial();
      waite();

    __asm__("cli");
      return test;

}




uint16_t floppyEncodeBlock(unsigned char side, unsigned char track, unsigned char block){

  return (side*1440)+(track*18+block);

}

uint8_t floppyDecodeBlock(uint16_t floppyEncodeBlock){
    uint8_t side = (floppyEncodeBlock - 1)/1440;
    uint16_t feb = floppyEncodeBlock - (side * 1440);
    uint8_t track = (feb - 1)/18;
    return feb - track * 18;
}

uint8_t floppyDecodeTrack(uint16_t floppyEncodeBlock){
    uint8_t side = (floppyEncodeBlock - 1)/1440;
    uint16_t feb = floppyEncodeBlock - (side * 1440);
    return (feb - 1)/18;
}

uint8_t floppyDecodeSide(uint16_t floppyEncodeBlock){
    
    return (floppyEncodeBlock - 1)/1440;
    
}




bool floppyChekBlockFree(uint16_t blockNum){
    uint16_t blockByte = 0;
    uint8_t blockBit  = 0;
    bool result = false;
    
    
    blockNum -= 1;
    blockByte = blockNum/8;
    blockBit  = 1<<(blockNum%8);
    result = false;
    
    if(((blockState[blockByte])&blockBit) == 0){
        result = true;
    }
    return result;
}

void floppyMakeBlockFree(uint16_t blockNum){
    uint16_t blockByte = 0;
    uint16_t blockBit  = 0;

    blockNum -= 1;
    blockByte = blockNum/8;
    blockBit  = 1<<(blockNum%8);


    blockState[blockByte] &= ~blockBit;
}

void floppyMakeBlockOccupide(uint16_t blockNum){
    uint16_t blockByte = 0;
    uint16_t blockBit  = 0;

    blockNum -= 1;
    blockByte = blockNum/8;
    blockBit  = 1<<(blockNum%8);


    blockState[blockByte] |= blockBit;
}

bool floppyLoadBlockState(){
  int i = 0;
  bool temp = FloppyLoadSector(0, 0, 1);
  waite();
    waite();
    waite();
    waite();

  for(i = 0;i<360;i++){
    blockState[i] = FloppyBuffer[i];
  }

  return temp;
}
bool floppySaveBlockState(){
  int i = 0;
  waite();
    waite();
    waite();
    waite();
  for(i = 0;i<360;i++){
    FloppyBuffer[i] = blockState[i];
  }

  return FloppySaveSector(0, 0, 1);
}


/*
FORMAT ERS

Sector 1: (first ever sector or block on disk) is BlockState sector that tells in what state is every block on disk. 0 is free and 1 is occupide





*/


// Function to convert minute, hour, day, month, and year to Unix timestamp
uint32_t dateToTimestamp(uint16_t minute, uint16_t hour, uint16_t day, uint16_t month, uint16_t year) {
    const uint32_t SECONDS_PER_MINUTE = 60;
    const uint32_t SECONDS_PER_HOUR = 3600;
    const uint32_t SECONDS_PER_DAY = 86400;
    const uint32_t DAYS_PER_YEAR = 365;
    uint32_t timestamp = 0;
    uint32_t totalDays = 0;
    uint16_t i = 0;

    // Calculate the number of days for each month (non-leap year)
    uint16_t daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Check if it's a leap year and adjust days in February
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysPerMonth[1] = 29;
    }

    // Calculate the number of years, months, and days
    for (i = 1970; i < year; i++) {
        if ((i % 4 == 0 && i % 100 != 0) || (i % 400 == 0)) {
            totalDays += 366;
        } else {
            totalDays += 365;
        }
    }

    for (i = 1; i < month; i++) {
        totalDays += daysPerMonth[i - 1];
    }

    totalDays += day - 1;

    // Calculate the Unix timestamp
    timestamp = totalDays * SECONDS_PER_DAY + hour * SECONDS_PER_HOUR + minute * SECONDS_PER_MINUTE;

    return timestamp;
}



void timestampToDate(uint32_t unixTimestamp, uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* hour, uint8_t* minute) {
    const uint8_t SECONDS_PER_MINUTE = 60;
    const uint16_t SECONDS_PER_HOUR = 3600;
    const uint32_t SECONDS_PER_DAY = 86400;
    const uint16_t DAYS_PER_YEAR = 365;

    uint32_t minutes;
    uint32_t hours;
    uint32_t days;
    uint8_t isLeapYear;
    uint8_t daysInFebruary;
    uint8_t daysPerMonthAdjusted[12];
    static const uint8_t daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t i = 0;
    minutes = unixTimestamp / SECONDS_PER_MINUTE;
    hours = minutes / 60;
    days = hours / 24;
    *year = 1970;

    while (days >= DAYS_PER_YEAR) {
        if ((*year % 4 == 0 && *year % 100 != 0) || (*year % 400 == 0)) {
            days -= 366;
        } else {
            days -= 365;
        }
        (*year)++;
    }

    isLeapYear = (*year % 4 == 0 && *year % 100 != 0) || (*year % 400 == 0);

    daysInFebruary = (isLeapYear) ? 29 : 28;

    for (i = 0; i < 12; i++) {
        daysPerMonthAdjusted[i] = daysPerMonth[i];
    }

    daysPerMonthAdjusted[1] = daysInFebruary;

    *month = 1;
    while (days >= daysPerMonthAdjusted[*month - 1]) {
        days -= daysPerMonthAdjusted[*month - 1];
        (*month)++;
    }

    *day = days + 1;
    *hour = (hours % 24);
    *minute = (minutes % 60);

    i = 2;


}