typedef int16_t fix8;


fix8 fix8_mul(fix8 a, fix8 b) {
    int32_t res = (int32_t)a * (int32_t)b;
    return res>>8;
}

fix8 fix8_div(fix8 a, fix8 b) {         //Not tested
    int32_t res = (int32_t)a/(int32_t)b;
    return res>>8;
}



fix8 fix8_from_int(int16_t x) {
    return (fix8)(x << 8);
}

int16_t fix8_to_int(fix8 x) {
    return (int16_t)x >> 8;
}




#define FIXED_POINT_SCALE 256

int fixedDec_to_int(fix8 fixed) {
    return fix8_mul(fixed, 0x6400)>>8;
    //return fix8_mul(0x0300,0x0200);
}


void int_to_string(int num, char *str) {
    int i = 0;
    int tmp_num;
    int digits;
    int rem;
    if(num < 0) {
        num = -num;
        str[i++] = '-';
    }
    if(num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    tmp_num = num;
    digits = 0;
    while(tmp_num > 0) {
        tmp_num /= 10;
        digits++;
    }
    str[i + digits] = '\0';
    while(num > 0) {
        rem = num % 10;
        str[i + digits - 1] = rem + '0';
        digits--;
        num = num / 10;
    }
}



void printFix(int x, int y, fix8 a){

  char tmpBuffer[7] = {0};
  int i = 0;
  int_to_string(a >> 8, tmpBuffer);

  while(tmpBuffer[i] > 47 && tmpBuffer[i] < 58){

    printChar(x,y,tmpBuffer[i]);


    x++;
    i++;
  }



  printChar(x,y,'.');
  x++;

  int_to_string(fixedDec_to_int(a & 0x00ff), tmpBuffer);
  printf(x,y,tmpBuffer);


}

void printInt(int x, int y, long a){

  char tmpBuffer[24] = {0};
  int_to_string(a, tmpBuffer);
  printf(x,y,tmpBuffer);
  }
