#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <bios.h>
#include <conio.h>
#include <dos.h>
#include <i86.h>
#include <io.h>
#include <sys/stat.h>
#include <malloc.h>

#include "vga.h"
#include "md.h"
#include "music.h"
#include "greets.h"
#include "vdptext.h"

#define FILE_MESSAGE " is missing or something."

const char* MAGICWORDS = "PRODUCED BY OR UNDER LICENSE FROM SEGA ENTERPRISES Ltd.\0";


const char barPaletteIndicesComp[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x15, 0x16 }; 
const char barPaletteIndices[] =     { 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x39, 0x3D, 0x3E };
const char barColours[] =            { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x0D, 0x0E }; // Don't use bright magenta for text lmao

unsigned char far *textScreen = (unsigned char far *)0xB8000000;

const char red[70]    = {0x23,0x26,0x29,0x2B,0x2E,0x30,0x33,0x35,0x37,0x39,0x3B,0x3C,0x3D,0x3E,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3E,0x3D,0x3C,0x3B,0x39,0x37,0x35,0x33,0x30,0x2E,0x2B,0x29,0x26,0x23,0x20,0x1D,0x1A,0x17,0x15,0x12,0x10,0xD,0xB,0x9,0x7,0x5,0x4,0x3,0x2,0x1,0x0,0x0,0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x7,0x9,0xB,0xD,0x10,0x12,0x15,0x17,0x1A,0x1D,0x20};
const char blue[70]   = {0x26,0x2B,0x30,0x35,0x39,0x3C,0x3E,0x3F,0x3F,0x3F,0x3D,0x3B,0x37,0x33,0x2E,0x29,0x23,0x1D,0x17,0x12,0xD,0x9,0x5,0x3,0x1,0x0,0x0,0x2,0x4,0x7,0xB,0x10,0x15,0x1A,0x20,0x26,0x2B,0x30,0x35,0x39,0x3C,0x3E,0x3F,0x3F,0x3F,0x3D,0x3B,0x37,0x33,0x2E,0x29,0x23,0x1D,0x17,0x12,0xD,0x9,0x5,0x3,0x1,0x0,0x0,0x2,0x4,0x7,0xB,0x10,0x15,0x1A,0x20};
const char green[70]  = {0x2B,0x35,0x3C,0x3F,0x3F,0x3B,0x33,0x29,0x1D,0x12,0x9,0x3,0x0,0x2,0x7,0x10,0x1A,0x26,0x30,0x39,0x3E,0x3F,0x3D,0x37,0x2E,0x23,0x17,0xD,0x5,0x1,0x0,0x4,0xB,0x15,0x20,0x2B,0x35,0x3C,0x3F,0x3F,0x3B,0x33,0x29,0x1D,0x12,0x9,0x3,0x0,0x2,0x7,0x10,0x1A,0x26,0x30,0x39,0x3E,0x3F,0x3D,0x37,0x2E,0x23,0x17,0xD,0x5,0x1,0x0,0x4,0xB,0x15,0x20};

char offset = 0;
char frame = 0;

char str[4] = "\0\0\0\0";    
char is80col;

unsigned int i, j, k, l, m, n;
unsigned long il, jk, kl, ml, nl;

unsigned char isRGB = 0;

int scrollLine = 0;
unsigned int scrollChar = 0;
unsigned char scrollPixel = 0;

unsigned char lastChar = 0;
unsigned char lastVideo = 0;

unsigned char controllerState = 0;

void fillScreen(){
    
    unsigned int textPos = (80 * 2 * 3) + 18;

    n = 0;
    for(m = 0; m < 16000; m += 2){
        if(m % 16 == 0) n++;
        if(m % 160 == 0) n = 0;
        textScreen[m] = 0xB0;
        textScreen[m+1] = barColours[n];
    }

    for(i = 0; i < greetsLen; i++){
        if(greets[i] == 0x0A){
            textPos += 36;
        } else if(greets[i] != 0x20){
            textScreen[textPos] = greets[i];
            textScreen[textPos + 1] = 0x0B;
        }
        textPos += 2;


    }
}

void makeItGrey(){
    for(i = 0; i < 10; i++){
        makeGrey(barPaletteIndices[i]);
    }
}

void cycleColours(){
    for(i = 0; i < 10; i++){
        j = (offset + i) % 70;
        outp(PALETTE_INDEX_WRITE, isRGB ? barPaletteIndices[i] : barPaletteIndicesComp[i]);
        outp(PALETTE_DATA, red[j] >> 1);
        outp(PALETTE_DATA, green[j] >> 1);
        outp(PALETTE_DATA, blue[j] >> 1);
    }
    offset++;
    if(offset == 70) offset = 0;
}

char checkFiles(){
    unsigned char fail = 0;
    // Check files exist.
    if(_access(MUSIC_FILE, R_OK) != 0) { fail++; printf("%s %s\n", MUSIC_FILE, FILE_MESSAGE); }
    if(_access(FONT_FILE, R_OK) != 0) { fail++; printf("%s %s\n", FONT_FILE, FILE_MESSAGE); }
    if(fail) return 0;
    return 1;
}

void toggleVideo(){
    if(lastVideo){
        MD_setVGA();
        lastVideo = 0;
    }else{
        MD_setVDP();
        lastVideo = 1;
    }
}

void doScrollingRT(){
    
    
    outp( CRTC_INDEX, PRESET_ROW_SCAN );
    outp( CRTC_DATA, scrollPixel & 0x0F );

}

void doScrolling(){

    if(scrollLine < 0) scrollLine = 0;
    if(scrollLine > 399) scrollLine = 399;
    scrollPixel = scrollLine % 16;
    scrollChar = (scrollLine / 16) * 80;

    outp( CRTC_INDEX, HIGH_ADDRESS );
    outp( CRTC_DATA, (unsigned char)((scrollChar & 0xFF00) >> 8) );
    outp( CRTC_INDEX, LOW_ADDRESS );
    outp( CRTC_DATA, (unsigned char)(scrollChar & 0xFF) );

}

// Application entrypoint
int main()
{
    
    setbuf(stdout, NULL);

    if(!checkFiles())
    {
        printf("Files are missing. Sadge.\n");
        return 0;
    }

    printf("Getting things ready...\n");
    printf("The magic words are %s\n", MAGICWORDS);

    if(!MUSIC_load()){
        return 1;
    }

    if(!MD_loadfont()){
        return 1;
    }
    
    if(!MD_setup()){
        return 1;
    }

    

    printf("LFG. Press a key to carry on.");
    getch();
    while(kbhit()) getch();
    
    getGrey();
    
    fadeOut();

    is80col = test80col();

    go80col();

    isRGB = MD_isRGB();

    if(isRGB) eightDots();
    // lineGraphicsOn();

    makeItGrey();
    cursorOff();
    fillScreen();
    
    MD_print(0,0, vdptext, vdptextLen);

    MUSIC_start();

    while(1){ // Main loop

        if(kbhit()) lastChar = getch();
        if(lastChar == 0x20){ // space
            toggleVideo();
            lastChar = 0;
        } 
        if(lastChar == 0x1B){ // escape
            break;
        }

        while(!(inp(INPUT_STATUS) & 0x08)); // Wait for Vretrace start
        
        // While hidden behind retrace, cycle colours
        frame++;
        if(frame % 5 == 0) cycleColours();
        if(frame == 70) frame = 0;

        // and do smooth scrolling
        doScrollingRT();

        while(inp(INPUT_STATUS) & 0x08); // Wait for retrace end

        if(frame % 2 == 0){ // read controllers every other frame because >60Hz breaks 6button pads apparently, and we could be at 70Hz
            controllerState = MD_readController();
            
            switch(controllerState){
                case 0x01: // up
                    scrollLine--;
                    break;
                case 0x02: // down
                    scrollLine++;
                    break;
                case 0x40: // a
                    MD_setVDP();
                    break;
                case 0x10: // b
                    MD_setVGA();
                    break;
            }

            doScrolling();
        }

        // While screen is drawing, read controller, talk to Z80/VDP/etc perhaps

    }
    
    is80col ? go80col() : go40col();

    MUSIC_stop();
    MUSIC_free();
    
    cursorOn();
    while(kbhit()) getch();
    return 0;
    
}


