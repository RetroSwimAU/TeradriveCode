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

#include "music.h"
#include "md.h"

void interrupt (*biosISR8)(void);			// Pointer to the BIOS interrupt service routine 8

unsigned char far *musicData = NULL;
unsigned long loopPos = 0x6B;
unsigned long currentPlaybackPos = 0x40;
unsigned char currentByte = 0;
unsigned long delayLeft = 0;
unsigned long fastTickRate = 0;
unsigned int biosCounter = 0;
unsigned long tickCounter = 0;

unsigned char MUSIC_load(){
    unsigned int i;
    long fileSize;
    FILE *musicFb;
    char musicByte;

    musicFb = fopen(MUSIC_FILE, "r");
    if(musicFb == NULL) return 1;
    fseek(musicFb, 0L, SEEK_END);
    fileSize = ftell(musicFb);
    rewind(musicFb);

    printf("Load the music (%u bytes) ", fileSize);
    
    musicData = _fmalloc(fileSize);

    if(musicData == NULL) {
        printf("Couldn't allocate memory. Not poggers.");
        return 0;
    }

    if(musicFb != NULL){
        for(i = 0; i < fileSize; i++){
            //printf("%u\n", il);
            fread(&musicByte, 1, 1, musicFb);
            musicData[i] = musicByte;
            if(i % 100 == 0) printf(".");
        }
        printf("\n");
    }else{
        printf("Couldn't open %s. Oof.", MUSIC_FILE);
        return 0;
    }

    return 1;
}

void MUSIC_free(){
    _ffree(musicData);
}

void MUSIC_start(){
    biosISR8 = _dos_getvect(8);
	_dos_setvect(8, timerHandler);
	outp(0x43, 0x36);
	// 40h = Frequency divider for counter #0
	outp(0x40, 0x91); // Low byte
	outp(0x40, 0x0A); // High byte
}

void MUSIC_stop()
{
	// Configure and set the timer, which sits at 40h-43h.
	outp(0x43, 0x36);
	// 40h = Frequency divider for counter #0 - Setting to zero puts us back to the default rate
	outp(0x40, 0x00);
	outp(0x40, 0x00);
	// Point ISR8 back to the original BIOS routine
	_dos_setvect(8, biosISR8);

    MD_updateWindow(PSG_ADDR);
    MD_memoryWriteB(PSG_ADDR, 0x9F);
    MD_memoryWriteB(PSG_ADDR, 0xBF);
    MD_memoryWriteB(PSG_ADDR, 0xDF);
    MD_memoryWriteB(PSG_ADDR, 0xFF);
}

void interrupt timerHandler(void)
{
	// Increment the counter cause the interrupt has happened
	tickCounter = tickCounter + 0x1B;

    MD_saveWindow();
    MD_updateWindow(PSG_ADDR);

    if(delayLeft > 0){
        delayLeft--;
    }else{
        currentByte = musicData[currentPlaybackPos];
        switch(currentByte){
            case 0x4F:
                currentPlaybackPos += 2;
                break;
            case 0x50:
                while(1){
                    MD_memoryWriteB(PSG_ADDR, musicData[currentPlaybackPos + 1]);
                    if(musicData[currentPlaybackPos + 2] == 0x50){
                        currentPlaybackPos += 2;
                    }else{
                        break;
                    }
                }
                currentPlaybackPos += 2;
                break;
            case 0x61:
                // delayLeft = ((unsigned int)musicData[currentByte + 1] | (unsigned int)musicData[currentByte + 2] << 8);
                currentPlaybackPos += 3;
                break;
            case 0x62:
                delayLeft += 4;
                currentPlaybackPos++;
                break;
            case 0x63:
                delayLeft += 9;
                currentPlaybackPos++;
                break;
            case 0x66:
                currentPlaybackPos = loopPos;
                break;
            default:
                currentPlaybackPos++;
        }   
    }

    MD_restoreWindow();
	
	// Since we have changed the timer rate, we have to determine when to call the BIOS ISR8 at its old rate.  This avoids the clock getting messed up.  While it may appear that biosCounter will only be less than fastTickRate one time, it will actually overflow back to 0 over time, making this a constant cycle.
	biosCounter += fastTickRate;
	if (biosCounter < fastTickRate)
	{
		biosISR8();
	}
	else
	{
		// If we aren't calling the BIOS ISR8 routine, we have to manually send an EOI (End of Interrupt) like it usually does.
		outp(0x20, 0x20);
	}
}
