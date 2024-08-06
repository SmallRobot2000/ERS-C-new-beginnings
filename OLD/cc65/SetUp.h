extern bool __fastcall__ isVDPIRQ();        //in ps2.s
uint8_t __TimeCunterCursor = 0;
uint8_t __TimeCunter = 0;
uint8_t __deltaTimeStamp = 0;
uint32_t publicunixTimestamp = 0;
bool __CursorState = false;
char KeyBuffer[256] = {0};
void initIO(){
    int i;
    int d;
    d = 12;
    EMem_int();
    setIrqErs();

    initVDP(1, 1, true, 1, 1, textMode);
    __asm__("cli");

    for (i = 0; i != 0x4000; i++) {
        VDP_W_VRAM(i,0x00);       //clear memory
}
    
    for (i = 0; i != 2048; i++) {
        VbufferPublic[i] = font[i];
}
    VDP_W_BLOCK(VbufferPublic, VDPpaterGeneratorAddress, 2048);
    __asm__("cli");




    colorBorderVDP(darkBlue, white);


    for(i = 0; i < 24; i++){
        for(n = 0;n<40;n++){
          printChar(n,i,0x20);
        }
}
    updateScreen();
    

   

}




void updateTimeStamp(){
  publicunixTimestamp += __deltaTimeStamp;
  __deltaTimeStamp = 0;

}

void interupt(){
    unsigned char cur_char;
    unsigned char KeyCode = getKeyCode();;

  if(isVDPIRQ() == true){
      __TimeCunter++;
      __TimeCunterCursor++;
      if(__TimeCunter == 50){
          __TimeCunter = 0;
          __deltaTimeStamp++;
      }
      if(__TimeCunterCursor == 16){
          __CursorState = !__CursorState;
          __CursorState &= 0x01;
          __TimeCunterCursor = 0;
      }

    return;
  }



  if((KeyCode == shiftCodeLeft || KeyCode == shiftCodeRight) && relesed == false){
    shifted = true;
    __asm__("cli");
    return;
  }

  if(KeyCode == releseCode){
    relesed = true;
    __asm__("cli");
    return;
  }

  

  

  if(relesed == true && (KeyCode == shiftCodeLeft || KeyCode == shiftCodeRight)){

  //later for relesing keys
    shifted = false;
    relesed = false;
    __asm__("cli");
    return;
  }
  if(relesed != false){
    relesed = false;
    __asm__("cli");
    return;   //Break if relesd and we didnt relesd important key just for now
    
  }
  

  if(shifted == true){

    cur_char = getKeyMapShifted(KeyCode);
  }else {

    cur_char = getKeyMap(KeyCode);
  }

    KeyBufferCounter++;
    KeyBuffer[KeyBufferCounter] = cur_char;

    
   
        //n++;
    __asm__("cli");

}