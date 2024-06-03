#include<stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>
#include <fcntl.h>
#include <share.h>
#include <i86.h>

char nextByte(int handle){
    return 0;
}

int main(void)
{
    int i = 0; 
    char c = 0;

    char far* magicword_buffer = 0x7E000000L; // Magic words must live at even memory address
                                              // If in <= 640KB area, bootstrap finds it "automatically"
                                              
    char far* psg_memory = 0xCE000011L; // PSG memory is $C00011 in 68K space.
                                        // PC -> 68K window described later.

    char* magicwords = "PRODUCED BY OR UNDER LICENSE FROM SEGA ENTERPRISES Ltd.\0"; // TMSS related?

    int len = strlen(magicwords);

    char result = 0;

    int handle = 0;
    int fileSuccess = 0;
    
    unsigned lenRead = 0;
    char buffer[1] = "";
    char thisChar = 0;
    char valueChar = 0;

    int filePos = 0;

    printf("Hello!\r\n\r\n");

    // int i = 0;

    printf("Write magic words to 7E00:0000\r\n"); // Probably a MUCH better way to do this.
    for( i = 0; i <= len; i++ ) {
        c = magicwords[i];
        printf("%d: %c \t", i, c);        
        *(magicword_buffer) = c;
        magicword_buffer++;
    }

    printf("\r\nPrepare 68K registers\r\n");

    outp(0x1160, 0x21); // Unclear
    outp(0x1163, 0x01); // Enable memory window PC->68K
    outp(0x1166, 0x00); // Default starting location for memory window,
    outp(0x1167, 0x00); // allows 'TMSS' code to find magic words in <=640KB region.

    printf("Engage.\r\n");
    outp(0x1164, 0x81); // Bit 0 releases 68K from reset.

    // Sometimes it hangs here. Powering off for a while usually fixes it though.
    // Probably some junk in registers I'm not aware of.

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
    outp(0x1163, 0x03); // Enable window
    
    // 8K window in 68K space begins at:
    //   (reg 0x1167 & 0x0F) << 20 
    // + (reg 0x1166 & 0xFE) << 12

    outp(0x1167, 0x0C);
    outp(0x1166, 0x00); 
    printf("PC RAM @ CE00:0000 points to 68K RAM @ $C00000\r\n");

    // My awful hackjob of reading a PSG VGM file.
    
    fileSuccess = _dos_open("test.vgm", O_RDONLY, &handle);
    if(fileSuccess != 0){
        printf("Couldn't open test.vgm: %d", fileSuccess);
    }

    printf("Reading test.vgm:\r\n");
    printf("000000");

    while(1)
    {
        printf("\b\b\b\b\b\b%6x", filePos);
        fileSuccess = _dos_read(handle, buffer, 1, &lenRead); // IS SLOOOOOOOOOOOOOOW
        if(fileSuccess != 0){
            printf("Couldn't read test.vgm: %d", fileSuccess);
            break;
        }
        if(lenRead == 0){
            printf("End of file");
            break;
        }
        if(filePos > 0x40){

            thisChar = buffer[0];

            if(thisChar == 0x4F){
                _dos_read(handle, buffer, 1, &lenRead); // Skip following byte
            }

            if(thisChar == 0x50){ // Next byte is the value to write to the PSG
                _dos_read(handle, buffer, 1, &lenRead);
                valueChar = buffer[0];
                *(psg_memory) = valueChar; // Pop
            }

            if(thisChar == 0x61){
                _dos_read(handle, buffer, 1, &lenRead); // Skip following byte
                _dos_read(handle, buffer, 1, &lenRead); // Skip following byte
            }

            if(thisChar == 0x62){
                // Do nothing
            }

            if(thisChar == 0x63){
                // Do nothing
            }

            if(thisChar == 0x66){
                break;
            }

        }
        filePos++;
        // delay(5); // Still nowhere near the correct clock for reading a PSG VGM.
    }
    _dos_close(handle);
    return 0;
}
