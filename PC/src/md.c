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

#include "md.h"

unsigned char last_upper, last_lower;

unsigned char far *mdMemory = (unsigned char far *)0xCE000000;
unsigned int far *mdMemoryW = (unsigned int far *)0xCE000000;
unsigned long far *mdMemoryL = (unsigned long far *)0xCE000000;

unsigned char far *pcBIOS = (unsigned char far *)0xF0000000;
char biosVersion[] = "79F2478";

unsigned char font[768];

unsigned char helloWorld[] = "Hello, world!";

const unsigned char VDP_SETUP[] = { 0x04, 0x04, 0x30, 0x3C, 0x07, 0x6C, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x81, 0x37, 0x00, 0x02, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00 };

unsigned long byteToDword(unsigned char in, unsigned char value){
	unsigned long thisLong = 0;
	unsigned char newValue = value & 0x0F;
	unsigned char mask = 0;
	unsigned long newLong = 0;

	int k;

	for(k = 0; k < 8; k++){
		mask = 1 << k;
		if(in & mask){
			newLong = (unsigned long)0x0000000F << (unsigned long)(k * 4);
			thisLong |= newLong;
		}
	}

	return thisLong;
		
}

void MD_init(){
    outp(0x1160, 0x21); // Selects 8KB ROM firmware page. 
    outp(0x1163, 0x01); // Enable ROM access from 68000.
    outp(0x1166, 0x00); // <-- These 2 registers being 00 makes 68000 search
    outp(0x1167, 0x00); // <-- entire 640KB for magic words.
}

void MD_boot68K(){
    outp(0x1164, 0x81); // Bit 0 on releases 68K from reset.
}

void MD_enableWindow(){
    outp(0x1163, inp(0x1163) | 0x02);
}

// Do this strategically.
void MD_updateWindow(long addr){
    unsigned char lower, upper;
    upper = (unsigned char)((addr & 0xF00000) >> 20);
    lower = (unsigned char)((addr & 0x0FE000) >> 12);
    if(upper != last_upper | lower != last_lower){
        outp(0x1167, upper);
        outp(0x1166, lower);
    }
}

void MD_setBase(unsigned char base){
    unsigned long newBase = (unsigned long)base << 24;
    mdMemory = (unsigned char far *)newBase;
    mdMemoryW = (unsigned int far *)newBase;
    mdMemoryL = (unsigned long far *)newBase;
}

// Endianness fixes apply for word/dword reads+writes. There is probably a faster way to do this but eh

void MD_memoryWriteB(unsigned long addr, unsigned char data){
    unsigned int offset = (unsigned int)(addr & 0x001FFF);
    mdMemory[offset] = data;
}

void MD_memoryWriteW(unsigned long addr, unsigned int data){
    unsigned int offset = (unsigned int)(addr & 0x001FFF) >> 1;
    unsigned int newData = data << 8 | data >> 8;
    mdMemoryW[offset] = newData;
}

void MD_memoryWriteL(unsigned long addr, unsigned long data){
    unsigned int offset = (unsigned int)(addr & 0x001FFF) >> 2;
    unsigned long newData = (data & 0xFF000000) >> 24 | (data & 0x00FF0000) >> 8 | (data & 0x0000FF00) << 8 | (data & 0x000000FF) << 24;
    mdMemoryL[offset] = newData;
}

int MD_setup(){
    unsigned char i;

    printf("\nOK, what are we working with here...\n");
    for(i = 0; i < 7; i++){
        if(pcBIOS[i] != biosVersion[i]) {
            printf("This ain't no TeraDrive. It ain't no country club either.\n");
            return 0;
        }
    }

    printf("\nSet TeraDrive registers...\n");

    MD_init();

    printf("Commence Blast Processing...\n");

    MD_boot68K();

    printf("...Blast Processing needs its coffee... ");
    for(i = 6; i > 0; i--){
        printf("\b%u", i - 1);
        delay(1000);
    }
    
    printf("\b\n");

    if(!(inp(0x1165) & 0x20)){
        printf("The 68000 didn't wake up. Is there a cartridge in the slot?");
        return 0;
    }

    printf("OK let's go.\n");

    MD_enableWindow();
    MD_setBase(inp(0x1162));

    printf("MD memory visible at %04X:%04X-%04X:%04X\n\n", FP_SEG(mdMemory), 
                FP_OFF(mdMemory), FP_SEG(mdMemory), FP_OFF(mdMemory) + 0x1FFF);

    MD_startVDP();

    return 1;
}

unsigned char MD_isRGB(){
    return inp(0x1165) & 0x04;
}

void MD_startVDP(){
    unsigned int i;
    unsigned long j;
    unsigned char k;
    unsigned char thisChar;
    unsigned char thisVDPByte;
    unsigned long thisVDPLong;

    // Initial settings
    MD_updateWindow(VDP_CONTROL);
    for(i = 0; i < VDP_SETUP_COUNT; i++){
        MD_memoryWriteW(VDP_CONTROL, 0x8000 | i << 8 | VDP_SETUP[i]);
    }

    // Setup palette - working OK I think!
    MD_memoryWriteL(VDP_CONTROL, VDP_COLOUR_0_REG);
    MD_memoryWriteW(VDP_DATA, VDP_COLOUR_0);
    MD_memoryWriteL(VDP_CONTROL, VDP_COLOUR_1_REG);
    MD_memoryWriteW(VDP_DATA, VDP_COLOUR_1);
    MD_memoryWriteL(VDP_CONTROL, VDP_COLOUR_2_REG);
    MD_memoryWriteW(VDP_DATA, VDP_COLOUR_2);
    MD_memoryWriteL(VDP_CONTROL, VDP_COLOUR_3_REG);
    MD_memoryWriteW(VDP_DATA, VDP_COLOUR_3);
    MD_memoryWriteL(VDP_CONTROL, VDP_COLOUR_15_REG);
    MD_memoryWriteW(VDP_DATA, VDP_COLOUR_15);

    MD_memoryWriteL(VDP_CONTROL, VDP_VRAM);

    // Load font to VRAM
    for(i = 0; i < 768; i++){
        thisChar = font[i];

        // 8 bits -> 4 bytes. E.g. b10010110 -> 0xF00F0FF0;
        thisVDPLong = byteToDword(thisChar, 0x0F);
        MD_memoryWriteL(VDP_DATA, thisVDPLong);
    }

    // This and the VRAM writes below seem to be good though.
    // It produces garbage, but it's garbage that follows the pattern of Hello World! haha
    MD_memoryWriteW(VDP_CONTROL, VDP_ENABLE_SCREEN);

    MD_print(0, 0, helloWorld, 13);

    // for(i = 128; i < 140; i++){
    //     j = 0xC000 + (i * 2);
    //     thisVDPLong = 0x40000000 | ((j & 0x3FFF) << 16) | ((j & 0xC000) >> 14);
    //     MD_memoryWriteL(VDP_CONTROL, thisVDPLong);
    //     MD_memoryWriteW(VDP_DATA, 0x0000 | (((unsigned int)(helloWorld[i - 128] - 32)) << 8) );
    // }

}

unsigned char MD_loadfont(){
    unsigned int i;
    long fileSize;
    FILE *fontFb;
    char fontByte;

    fontFb = fopen(FONT_FILE, "r");
    if(fontFb == NULL) return 1;
    fseek(fontFb, 0L, SEEK_END);
    fileSize = ftell(fontFb);
    rewind(fontFb);

    printf("Load the font (%u bytes) ", fileSize);
    
    if(fontFb != NULL){
        for(i = 0; i < fileSize; i++){
            //printf("%u\n", il);
            fread(&fontByte, 1, 1, fontFb);
            font[i] = fontByte;
            if(i % 100 == 0) printf(".");
        }
        printf("\n");
    }else{
        printf("Couldn't open %s. Bummer", FONT_FILE);
        return 0;
    }

    return 1;
}

void MD_print(unsigned char x, unsigned char y, unsigned char* text, unsigned int len){
    unsigned int i;
    unsigned long j;
    unsigned long thisVDPLong;
    unsigned int thisVDPInt;

    unsigned int offset = x * 2 + y * 128;

    MD_updateWindow(VDP_CONTROL);

    for(i = 0; i < len; i++){
        j = 0xC000 + offset + (i * 2);
        thisVDPLong = VDP_VRAM | ((j & 0x3FFF) << 16) | ((j & 0xC000) >> 14);
        MD_memoryWriteL(VDP_CONTROL, thisVDPLong);
        MD_memoryWriteW(VDP_DATA, 0x0000 | (unsigned int)(helloWorld[i] - 32));
    }

}
