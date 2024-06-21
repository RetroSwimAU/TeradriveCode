#include <i86.h>
#include <conio.h>
#include "vga.h"

unsigned char greyR, greyG, greyB;

void eightDots(){
    char r;
    r = inp(MISC_READ);
    r &= ~0x0C;
    outp(MISC_OUTPUT, r);
    
    outp(SC_INDEX, 0x01);
    r = inp(SC_DATA);
    r |= 0x01;
    outp(SC_INDEX, 0x01);
    outp(SC_DATA, r);
    
    outp(AC_WRITE, 0x20);

}

void lineGraphicsOn(){
    char r;
    inp(INPUT_STATUS);
    outp(AC_WRITE, 0x10);
    r = inp(AC_READ);
    inp(INPUT_STATUS);
    outp(AC_WRITE, 0x10);
    outp(AC_WRITE, r & ~0x04);
    inp(INPUT_STATUS);
}

char test80col(){
    union REGS getVidMode;
    getVidMode.h.ah = 0x0F;
    int86(0x10, &getVidMode, &getVidMode);
    if(getVidMode.h.al == 0x03) return 1;
    return 0;
}

void go80col(){
    union REGS setVidMode;
    setVidMode.h.ah = 0x00;
    setVidMode.h.al = 0x03;
    int86(0x10, &setVidMode, &setVidMode);
}

void go40col(){
    union REGS setVidMode;
    setVidMode.h.ah = 0x00;
    setVidMode.h.al = 0x01;
    int86(0x10, &setVidMode, &setVidMode);
}

void getGrey(){
    outp(PALETTE_INDEX_READ, 0x07);
    greyR = inp(PALETTE_DATA);
    greyG = inp(PALETTE_DATA);
    greyB = inp(PALETTE_DATA);
}

void makeGrey(char index){
    outp(PALETTE_INDEX_WRITE, index);
    outp(PALETTE_DATA, greyR);
    outp(PALETTE_DATA, greyG);
    outp(PALETTE_DATA, greyB);
}

void cursorOff(){
    char r;
    outp(CRTC_INDEX, 0x0A);
    r = inp(CRTC_DATA);
    outp(CRTC_INDEX, 0x0A);
    outp(CRTC_DATA, r | 0x20);
}

void cursorOn(){
    char r;
    outp(CRTC_INDEX, 0x0A);
    r = inp(CRTC_DATA);
    outp(CRTC_INDEX, 0x0A);
    outp(CRTC_DATA, r & ~0x20);
}

void fadeOut(){
    unsigned char thisGrey = greyR;
    while(thisGrey > 0){
        while(!(inp(INPUT_STATUS) & 0x08));
        outp(PALETTE_INDEX_WRITE, 0x07);
        outp(PALETTE_DATA, thisGrey);
        outp(PALETTE_DATA, thisGrey);
        outp(PALETTE_DATA, thisGrey);
        while(inp(INPUT_STATUS) & 0x08);
        thisGrey--;
    }
}
