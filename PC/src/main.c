/* Copyright 2021 Intbeam

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

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

const char* MAGICWORDS = "PRODUCED BY OR UNDER LICENSE FROM SEGA ENTERPRISES Ltd.\0";
const char* MUSICFILE = "sidecrwl.vgm";

const char barPaletteIndices[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x39, 0x3D, 0x3E }; // To restore afterwards.
const char barColours[] =        { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x0D, 0x0E }; // Don't use bright magenta for text lmao

unsigned char far *textScreen = (unsigned char far *)0xB8000000;
unsigned char far *musicData = NULL;

const char red[70]    = {0x23,0x26,0x29,0x2B,0x2E,0x30,0x33,0x35,0x37,0x39,0x3B,0x3C,0x3D,0x3E,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3E,0x3D,0x3C,0x3B,0x39,0x37,0x35,0x33,0x30,0x2E,0x2B,0x29,0x26,0x23,0x20,0x1D,0x1A,0x17,0x15,0x12,0x10,0xD,0xB,0x9,0x7,0x5,0x4,0x3,0x2,0x1,0x0,0x0,0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x7,0x9,0xB,0xD,0x10,0x12,0x15,0x17,0x1A,0x1D,0x20};
const char blue[70]   = {0x26,0x2B,0x30,0x35,0x39,0x3C,0x3E,0x3F,0x3F,0x3F,0x3D,0x3B,0x37,0x33,0x2E,0x29,0x23,0x1D,0x17,0x12,0xD,0x9,0x5,0x3,0x1,0x0,0x0,0x2,0x4,0x7,0xB,0x10,0x15,0x1A,0x20,0x26,0x2B,0x30,0x35,0x39,0x3C,0x3E,0x3F,0x3F,0x3F,0x3D,0x3B,0x37,0x33,0x2E,0x29,0x23,0x1D,0x17,0x12,0xD,0x9,0x5,0x3,0x1,0x0,0x0,0x2,0x4,0x7,0xB,0x10,0x15,0x1A,0x20};
const char green[70]  = {0x2B,0x35,0x3C,0x3F,0x3F,0x3B,0x33,0x29,0x1D,0x12,0x9,0x3,0x0,0x2,0x7,0x10,0x1A,0x26,0x30,0x39,0x3E,0x3F,0x3D,0x37,0x2E,0x23,0x17,0xD,0x5,0x1,0x0,0x4,0xB,0x15,0x20,0x2B,0x35,0x3C,0x3F,0x3F,0x3B,0x33,0x29,0x1D,0x12,0x9,0x3,0x0,0x2,0x7,0x10,0x1A,0x26,0x30,0x39,0x3E,0x3F,0x3D,0x37,0x2E,0x23,0x17,0xD,0x5,0x1,0x0,0x4,0xB,0x15,0x20};

char offset = 0;
char frame = 0;

char str[4] = "\0\0\0\0";    
char is80col;

unsigned int i, j, k, l, m, n;
unsigned long il, jk, kl, ml, nl;


void fillScreen(){
    n = 0;
    for(m = 0; m < 16000; m += 2){
        if(m % 16 == 0) n++;
        if(m % 160 == 0) n = 0;
        textScreen[m] = 0xB0;
        textScreen[m+1] = barColours[n];
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
        outp(PALETTE_INDEX_WRITE, barPaletteIndices[i]);
        outp(PALETTE_DATA, red[j] >> 1);
        outp(PALETTE_DATA, green[j] >> 1);
        outp(PALETTE_DATA, blue[j] >> 1);
    }
    offset++;
    if(offset == 70) offset = 0;
}

char checkFiles(){
    // Check files exist.
    if(_access("sidecrwl.vgm", R_OK) != 0) return 0;
    return 1;
}

// Application entrypoint
int main()
{
    long fileSize;
    FILE *musicFb;
    char musicByte;

    setbuf(stdout, NULL);

    if(!checkFiles())
    {
        printf("Files are missing. Sadge.\n");
        return 0;
    }

    printf("Getting things ready...\n");
    printf("The magic words are %s\n", MAGICWORDS);

    musicFb = fopen(MUSICFILE, "r");
    if(musicFb == NULL) return 1;
    fseek(musicFb, 0L, SEEK_END);
    fileSize = ftell(musicFb);
    rewind(musicFb);

    printf("Load the music (%u bytes) ", fileSize);
    
    musicData = _fmalloc(fileSize);

    if(musicData == NULL) {
        printf("Couldn't allocate memory. Not poggers.");
        return 1;
    }

    if(musicFb != NULL){
        for(il = 0; il < fileSize; il++){
            //printf("%u\n", il);
            fread(&musicByte, 1, 1, musicFb);
            musicData[il] = musicByte;
            if(il % 100 == 0) printf(".");
        }
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
    if(MD_isRGB()) eightDots();
    // lineGraphicsOn();

    makeItGrey();
    cursorOff();
    fillScreen();
    
    while(!kbhit()){ // Main loop

        while(!(inp(INPUT_STATUS) & 0x08)); // Wait for Vretrace start
        
        // While hidden behind retrace, cycle colours
        frame++;
        if(frame % 5 == 0) cycleColours();
        if(frame == 70) frame = 0;

        // Play some PSG here.

        while(inp(INPUT_STATUS) & 0x08); // Wait for retrace end

        // While screen is drawing, read controller, talk to Z80/VDP/etc perhaps

    }
    
    is80col ? go80col() : go40col();

    hfree(musicData);

    cursorOn();
    while(kbhit()) getch();
    return 0;
    
}


