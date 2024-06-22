/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include "teradrive.h"
#include <resources.h>

vu8 *pb; // Main utility RAM pointer to a byte
    
const char bounce[69] = {0,2,5,7,9,11,14,16,18,20,23,25,27,29,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,63,65,67,69,70,72,73,75,77,78,79,81,82,83,85,86,87,88,89,90,91,92,93,94,95,95,96,97,97,98,98,99,99,99,99,100,100,100,100};
//const char bounce[69] = {0,4,10,14,18,22,28,32,36,40,46,50,54,58,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,126,130,134,138,140,144,146,150,154,156,158,162,164,166,170,172,174,176,178,180,182,184,186,188,190,190,192,194,194,196,196,198,198,198,198,200,200,200,200};

const char pcColours [24] = {0x13,	0x20,	0x2E,
0x17,	0x22,	0x2E,
0x1B,	0x25,	0x2F,
0x1F,	0x27,	0x2F,
0x23,	0x29,	0x30,
0x27,	0x2B,	0x30,
0x2B,	0x2E,	0x30,
0x2C,	0x2E,	0x30};

const char pcColourOrder[16] = { 0,1,2,3,4,5,6,7,7,6,5,4,3,2,1 };

char colourOffset = 0;

char seconds = 0;
char minutes = 0;
char hours = 0;

char isVGAGraphical = 0;
char frameCol = 0;
char isRearSwitchRGB = 0;

void clearVGA()
{
    u16 j;
    
    for(j = 0; j < 0xFFFF; j++){
        PC_memoryWriteFlatB(0xA0000 + j, 0);
    }

    for(j = 0; j < 0x8000; j++){
        PC_memoryWriteFlatB(0xB8000 + j, 0);
    }
}

void PostCardWrite(char code){
    PC_IOPortWriteB(0x80, code);
}

void rotateColours(){
    s8 c;
    u8 l; 
    int thisColour = 0;
    int cRGB;
    
    
    PC_IOPortWriteB(0x3c8, 0xCF);
    
    for(c = 0; c < 16; c++){
        thisColour = colourOffset + c;
        if(thisColour > 15) thisColour -= 16;
        cRGB = pcColourOrder[thisColour] * 3;
        for(l = 0; l < 3; l++){
            PC_IOPortWriteB(0x3c9, pcColours[cRGB]);
            PC_IOPortWriteB(0x3c9, pcColours[cRGB + 1]);
            PC_IOPortWriteB(0x3c9, pcColours[cRGB + 2]);
        }
    }

    colourOffset++;
    if(colourOffset == 16) colourOffset = 0;
}

void ReadRTC(){
    PC_IOPortWriteB(0x70, 0x00);
    seconds = PC_IOPortReadB(0x71);
    PC_IOPortWriteB(0x70, 0x02);
    minutes = PC_IOPortReadB(0x71);
    PC_IOPortWriteB(0x70, 0x04);
    hours = PC_IOPortReadB(0x71);
}

// For the BIOS post card, so it shows 0-60 for frame counter.
char charToBCD(char i){
    char j = i / 10;
    return (char)(j * 16 + (i % 10)) & 0xFF;
}

void ButtonA(){

    u8 c;
    u16 j = 0;
    VDP_drawText("A", 39, 0);
    PC_goVGA13h(isRearSwitchRGB);
    isVGAGraphical = 1;

    // Setup Palette
    char thisPal = 0;
    PC_IOPortWriteB(0x3c8, 0x10);
    for(c = 0; c < 240; c++){
        if(c <= 64) {
            PC_IOPortWriteB(0x3c9, thisPal);
            PC_IOPortWriteB(0x3c9, 0x00);
            PC_IOPortWriteB(0x3c9, 0x00);
        } else if (c <= 128){
            PC_IOPortWriteB(0x3c9, 0x00);
            PC_IOPortWriteB(0x3c9, thisPal);
            PC_IOPortWriteB(0x3c9, 0x00);
        } else if (c <= 192){
            PC_IOPortWriteB(0x3c9, 0x00);
            PC_IOPortWriteB(0x3c9, 0x00);
            PC_IOPortWriteB(0x3c9,thisPal);
        } else {
            PC_IOPortWriteB(0x3c9, thisPal);
            PC_IOPortWriteB(0x3c9, thisPal);
            PC_IOPortWriteB(0x3c9, thisPal);
        }
        thisPal++;
        if(c % 0x40 == 0) thisPal = 0;
        if(c == 192) thisPal = 16;
    }

    // Draw image slowly
    char thisColour = 0;
    for(j = 0; j < 0xFA00; j++){
        if(j % 320 == 0 ) thisColour = 0;
        PC_memoryWriteFlatB(0xA0000 + j,thisColour);
        if(j % 320 < 255) thisColour++;
    }
    
    

}

void ButtonB(){
    u16 j;
    VDP_drawText("B", 39, 0);
    
    PC_goVGAText80(isRearSwitchRGB);
    frameCol = 78;
    isVGAGraphical = 0;

    clearVGA();

    PC_drawVGAText("Hello world!", 13, 0, 0);

    for(j = 0; j < sizeof(greets); j++){
        PC_memoryWriteB(0xB800, j + 160, greets[j]);
    }
}

void ButtonC(){
    u16 j;
    VDP_drawText("C", 39, 0);
    
    PC_goVGAText40(isRearSwitchRGB);
    frameCol = 38;
    isVGAGraphical = 0;

    clearVGA();

    PC_drawVGAText("Hello world!", 13, 0, 0);

    for(j = 0; j < sizeof(credits); j++){
        PC_memoryWriteB(0xB800, j + 80, credits[j]);
    }
}



void ButtonUP(){
    VDP_drawText("U", 39, 0);
    VDP_drawText("VDP",20,7);
    TD_setOutputVDP();
}

void ButtonDOWN(){
    VDP_drawText("D", 39, 0);
    VDP_drawText("VGA",20,7);
    TD_setOutputVGA();
}



int main()
{
    bool checkPass = false;
    char i = 0;
    char str[3] = "\0\0\0";
    char time[9] = "00:00:00\0";
    
    u16 colsPerRow = 40;
    u16 row = 0;
    u16 col = 0;
    char textLine[40];
    u16 j = 0;

    s16 bounceTrack = 0;
    s8 bounceDir = 1;
    s16 movePos = 0;
    s8 moveDir = 1;
    u16 joyState = 0;

    Sprite* sanic;

    VDP_drawText("Please wait...", 0, 0);
    pb = (u8*) TD_SHARED_REGISTER_5;
    if(*pb && TD_SR5_VIDEO_SWITCH) {
        isRearSwitchRGB = 1;
        frameCol = 78;
    }else{
        frameCol = 38;
    }
    SYS_doVBlankProcess();

    // PC Setup
    PC_drawVGAText("Hello world!", 13, 0, 0);

    SPR_init();
    JOY_init();

    PAL_setPalette(PAL2, sanic_spr.palette->data, DMA);
    sanic = SPR_addSprite(&sanic_spr, 0, 200, TILE_ATTR(PAL2, true, false, true));

    // Static text
    //           "1234567890123456789012345678901234567890"
    VDP_drawText("TeraDrive Demo 0.5 - RetroSwim 2024", 0, 0);
    VDP_drawText("Frame:", 0, 1);
    VDP_drawText("Time:", 0, 2);
    VDP_drawText("PC BIOS F0000-F0050:", 0, 3);
    
    // Print BIOS ident, and warn if this isn't a TeraDrive.
    for(j = 0; j < 80; j++){
            row = 4 + (j / colsPerRow);
            col = j % colsPerRow;
            textLine[col] = PC_memoryReadB(0xF000, j);
            if(col == colsPerRow - 1) {
                if((textLine[23] == 'I' && textLine[24] == 'B' && textLine[25] == 'M') |
                   (textLine[23] == 'S' && textLine[24] == 'E' && textLine[25] == 'R'))
                {
                    VDP_drawText(textLine, 0, row);
                    checkPass = true;
                }else{
                    PAL_setColor(15,RGB24_TO_VDPCOLOR(0xFF0000));
                    VDP_drawText("** This doesn't seem to be a TeraDrive!!", 0, row);
                }
                
            }
    }

    if(checkPass){ //"0123456789012345678901234567890123456789"
        if(isRearSwitchRGB){
        VDP_drawText("TeraDrive detected! VDP->RGB, Mode=RGB",0,7);
        }else{
        VDP_drawText("TeraDrive detected! VDP->RGB, Mode=Video",0,7);
        }
        VDP_drawText("VGA A=Mode 13h B=80col Txt C=40col txt",0,8);
        VDP_drawText("Output Up=VDP Down=VGA",0,9);
        XGM_setLoopNumber(-1);
        XGM_startPlay(&goodMusic);
    }else{
        XGM_setLoopNumber(-1);
        XGM_startPlay(&badMusic);
    }

    // Initialise RTC to 00:00:00 if it doesn't already contain a valid time.
    ReadRTC();

    if(hours == -1){
        PC_IOPortWriteB(0x70, 0x00);
        PC_IOPortWriteB(0x71, 0x00);
        PC_IOPortWriteB(0x70, 0x02);
        PC_IOPortWriteB(0x71, 0x00);
        PC_IOPortWriteB(0x70, 0x04);
        PC_IOPortWriteB(0x71, 0x00);
    }
    
    // Wait til next VSync to start game loop
    VDP_waitVSync();

    while(1)
    {   
        // Move sanic
        if(checkPass){    
            SPR_setPosition(sanic, movePos, 200 - bounce[bounceTrack]);

            bounceTrack += bounceDir;
            if(bounceTrack == 68) bounceDir = -1;
            if(bounceTrack == 0) bounceDir = 1;

            movePos += moveDir;
            if(movePos == 300) moveDir = -1;
            if(movePos == 0) moveDir = 1;
            
        } else {
            if(i % 30 == 0){
                moveDir = -moveDir;
            }
        }
        SPR_setHFlip(sanic, moveDir == -1);
        SPR_update();

        // Print frame # and write to POST card
        sprintf(str, "%02d", i);
        VDP_drawText(str, 7, 1);
        PC_drawVGAText(str, 2, frameCol, 0);
        if(i % 10 == 0 && isVGAGraphical) rotateColours();
        PostCardWrite(charToBCD(i));

        // Print RTC time. Values from RTC are in BCD.
        ReadRTC();
        sprintf(time, "%02x:%02x:%02x", hours, minutes, seconds);
        VDP_drawText(time, 6, 2);

        // Increment frame counter
        i++;
        if(i == 60){
            i = 0;
        }
        
        SYS_doVBlankProcess();

        joyState = JOY_readJoypad(JOY_1);
        if(joyState & BUTTON_A){
            ButtonA();
        }
        if(joyState & BUTTON_B){
            ButtonB();
        }
        if(joyState & BUTTON_C){
            ButtonC();
        }
        if(joyState & BUTTON_UP){
            ButtonUP();
        }
        if(joyState & BUTTON_DOWN){
            ButtonDOWN();
        }
            
    }
    return (0);
}
