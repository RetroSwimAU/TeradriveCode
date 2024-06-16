#include <types.h>
#include "teradrive.h"
#include "vga.h"

u8 isFontBackedUp = 0;
u8 isFontRestored = 0;

void PC_memoryWriteFlatB(int address, char value){
    vu8 *pb;
    pb = (u8*) PC_MEMORY_BASE + address;
    *pb = value;
}

void PC_memoryWriteB(u16 segment, u16 offset, char value){
    long flatAddress = (segment << 4) + offset;
    PC_memoryWriteFlatB(flatAddress, value);
}

char PC_memoryReadFlatB(int address){
    vu8 *pb;
    pb = (u8*) PC_MEMORY_BASE + address;
    return *pb;
}

char PC_memoryReadB(u16 segment, u16 offset){
    long flatAddress = (segment << 4) + offset;
    return PC_memoryReadFlatB(flatAddress);
}


void PC_IOPortWriteB(int port, char value){
    vu8 *pb;
    pb = (u8*) PC_IO_BASE + port;
    *pb = value;
}

void PC_IOPortWriteW(int port, u16 value){
    vu8 *pb;
    pb = (u8*) PC_IO_BASE + port;
    char lower = value & 0xFF;
    char upper = (value & 0xFF00) >> 8;
    *pb = lower;
    *pb = upper;
}

char PC_IOPortReadB(int port){
    vu8 *pb;
    pb = (u8*) PC_IO_BASE + port;
    return (char)*pb;
}

void PC_drawVGAText(char * str, int len, u8 x, u8 y){
    int start = PC_MEMORY_BASE + 0xB8000 + (x * 2) + (y * 160);
    vu8 *pb;
    for(int i = 0; i < len; i++){
        pb = (u8*) start + (i * 2);
        *pb = str[i];
        pb = (u8*) start + (i * 2) + 1;
        *pb = 0x07;
    }
}


void PC_goVGAText80(u8 isRGB){
    if(isFontBackedUp) {
        if(!isFontRestored){
            VGA_fontRestore();
            VGA_paletteRestore();
            isFontRestored = 1;
        }
    }
    VGA_mode(0x03, isRGB);
}

void PC_goVGAText40(u8 isRGB){
    if(isFontBackedUp) {
        if(!isFontRestored){
            VGA_fontRestore();
            VGA_paletteRestore();
            isFontRestored = 1;
        }
    }
    VGA_mode(0x01, isRGB);
}

void PC_goVGA13h(u8 isRGB){
    VGA_mode(0x13, isRGB);
    if(!isFontBackedUp) {
        VGA_fontBackup();
        VGA_paletteBackup();
    }
    isFontBackedUp = 1;
    isFontRestored = 0;
}

void TD_exposeMemory(int address){
    vu16 *pb;
    int upper = (address & 0xF00000) >> 20;
    int lower = (address & 0x0FE000) >> 12;
    pb = (u16*) TD_SHARED_REGISTER_6;
    *pb = (lower << 8) + upper;
}

void TD_setOutputVDP(){
    vu8 *pb;
    pb = (u8*) TD_SHARED_REGISTER_4;
    *pb = *pb | TD_SR4_VIDEO_SOURCE;
}

void TD_setOutputVGA(){
    vu8 *pb;
    pb = (u8*) TD_SHARED_REGISTER_4;
    *pb = *pb & ~TD_SR4_VIDEO_SOURCE;
}