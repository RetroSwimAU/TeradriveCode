#include<stdio.h>
#include <stdlib.h>

// Required by something (TMSS?) for 68K bootloader to allow access to resources from PC side.
char * const MAGICWORDS = "PRODUCED BY OR UNDER LICENSE FROM SEGA ENTERPRISES Ltd.\0";
int const MAGICWORDSLENGTH = 55;

int main(void)
{
    int i = 0; 
    char c = 0;
                                          
    char far* psg_memory = 0xCE000011L; // PSG memory is $C00011 in 68K space.
                                        // PC -> 68K window described later.

    int magicWordsPos = MAGICWORDS;  // Is meant to show the RAM adderess of the magic words, but not sure it's accurate.

    char result = 0;

    FILE *fp;
    int lenRead = 0;
    char thisChar = 0;
    
    int filePos = 0;

    
    printf("Hello!\r\n\r\n");

    printf("Magic words at %x\r\n", magicWordsPos);
    for( i = 0; i <= MAGICWORDSLENGTH; i++ ) {
        c = MAGICWORDS[i];
        printf("%d: %c %2x \t", i, c, c);
    }

    if(magicWordsPos & 1) printf("WARN: Magic word address doesn't seem to be even. 68k wakeup may fail.\r\n");
    
    printf("\r\nPrepare 68K registers\r\n");

    outp(0x1160, 0x21); // Unclear what this does
    outp(0x1163, 0x01); // Enable memory window PC->68K
    outp(0x1166, 0x00); // Default starting location for memory window,
    outp(0x1167, 0x00); // allows 'TMSS' code to find magic words in <=640KB region.

    printf("Engage.\r\n");
    outp(0x1164, 0x81); // Bit 0 on releases 68K from reset.

    // Sometimes it hangs here. Powering off for a while usually fixes it though.
    // Probably some junk in registers I'm not aware of.
    // 04-Jun-24: No longer appears to hang, seems there was conflict with my PicoMEM card's BIOS base address.

    // 68k now executes a bootloader and returns control to the PC.
    
    printf("Wait for 68k wake up...\r\n");
    sleep(5);

    // TMSS screen temporarily obscures PC screen.
    
    result = inp(0x1165);
    if(result & 0x20){ // Bit 5 of 0x1165 indicates hardware is unlocked.
        printf("0x1165 = %X: 68k is awake\r\n", result);
    }else{
        printf("0x1165 = %X: 68k seems asleep...\r\n", result);
        return 1;
    }

    outp(0x1162, 0xCE); // Should already be CE. Determines the address in PC space of an 8k window to 68K space. 
    outp(0x1163, 0x03); // Enable said window
    
    // 8K window in 68K space begins at:
    //   (reg 0x1167 & 0x0F) >> 20 
    // + (reg 0x1166 & 0xFE) >> 12

    outp(0x1167, 0x0C);
    outp(0x1166, 0x00); 
    printf("PC RAM @ CE00:0000 points to 68K RAM @ $C00000\r\n");

    // My awful hackjob of reading a PSG VGM file.
    // 04-Jun-24: Sped up greatly using fread() instead of DOS routines. Still a bit wonky.
    
    fp = fopen("test.vgm","r");

    if (fp == NULL) {
        printf("Couldn't open test.vgm");
        return 1;
    }

    printf("Reading test.vgm:\r\n");
    printf("File pos: 000000");

    while(1)
    {
        printf("\b\b\b\b\b\b%6x", filePos);

        lenRead = fread(&thisChar, 1, 1, fp);
        if(lenRead == 0){
            printf("EoF or other error. Exit.");
            break;
        }
        
        if(filePos > 0x40){

            if(thisChar == 0x4F){                                  // Game Gear PSG Stereo command
                lenRead = fread(&thisChar, 1, 1, fp);              // Skip following byte
            }

            if(thisChar == 0x50){                                  // PSG command
                lenRead = fread(&thisChar, 1, 1, fp);              // Read following byte
                *(psg_memory) = thisChar;                          // Write to PSG
            }

            if(thisChar == 0x61){                                  // VGM Pause w/ params
                lenRead = fread(&thisChar, 1, 1, fp);              // Skip following byte
                lenRead = fread(&thisChar, 1, 1, fp);              // Skip following byte
            }

            if(thisChar == 0x62){                                  // VGM Pause
                // Do nothing
            }

            if(thisChar == 0x63){                                  // VGM Pause
                // Do nothing
            }

            if(thisChar == 0x66){                                  // VGM EoF
                break;
            }

        }
        filePos++;
    }

    fclose(fp);
    return 0;
}
