#include <conio.h>
#include <stdio.h>
#include <i86.h>
#include "md.h"

unsigned char last_upper, last_lower;

unsigned char far *mdMemory = (unsigned char far *)0xCE000000;
unsigned int far *mdMemoryW = (unsigned int far *)0xCE000000;

unsigned char far *pcBIOS = (unsigned char far *)0xF0000000;
char biosVersion[] = "79F2478";

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
}

void MD_memoryWriteB(unsigned long addr, unsigned char data){
    unsigned long offset = (unsigned int)(addr & 0x001FFF);
    mdMemory[offset] = data;
}

void MD_memoryWriteW(unsigned long addr, unsigned int data){
    unsigned int offset = (unsigned int)(addr & 0x001FFF);
    mdMemoryW[offset] = data;
}

int MD_setup(){
    int i;

    printf("\nOK, what are we working with here...\n");
    for(i = 0; i < 7; i++){
        if(pcBIOS[i] != biosVersion[i]) {
            printf("This ain't no TeraDrive. It ain't no country club either.\n");
            return 0;
        }
    }

    printf("\nSet TeraDrive registers...\n");

    outp(0x1160, 0x21); // Selects 8KB ROM firmware page. 
    outp(0x1163, 0x01); // Enable ROM access from 68000.
    outp(0x1166, 0x00); // <-- These 2 registers being 00 makes 68000 search
    outp(0x1167, 0x00); // <-- entire 640KB for magic words.

    printf("Commence Blast Processing...\n");
    outp(0x1164, 0x81); // Bit 0 on releases 68K from reset.

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

    MD_setBase(inp(0x1162));

    printf("MD memory visible at %04X:%04X-%04X:%04X\n\n", FP_SEG(mdMemory), 
                FP_OFF(mdMemory), FP_SEG(mdMemory), FP_OFF(mdMemory) + 0x1FFF);

    return 1;
}

unsigned char MD_isRGB(){
    return inp(0x1165) & 0x04;
}
