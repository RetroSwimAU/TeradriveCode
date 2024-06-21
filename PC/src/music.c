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

unsigned char far *musicData = NULL;


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
