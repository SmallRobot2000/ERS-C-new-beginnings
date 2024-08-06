#include "fix8.h"




void putpixel(unsigned char x,unsigned char y){

  unsigned int address = ((x>>3)*8)+(y&0x07)+((y>>3)*256);
  unsigned char data = 128>>(x&0x07);
  VbufferPublic[address] = VbufferPublic[address]|data;

  

}



void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;
    while (x1 != x2 || y1 != y2) {
        putpixel(x1, y1);
        e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}




void drawRectangle(int x0, int y0, int x1, int y1){

  drawLine(x0,y0,x1,y0);
  drawLine(x0,y0,x0,y1);

  drawLine(x0,y1,x1,y1);
  drawLine(x1,y1,x1,y0);


}







void darwStaticCube(x,y,size,offset){
  drawLine(x,y,x+offset,y-offset);
  drawLine(x+size,y+size,x+size+offset,y+size-offset);

  drawLine(x,y+size,x+offset,y-offset+size);
  drawLine(x+size,y,x+size+offset,y-offset);

  drawRectangle(x,y,x+size,y+size);
  drawRectangle(x+offset,y-offset,x+size+offset,y+size-offset);
}



