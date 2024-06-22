#define MUSIC_FILE "sidecrwl.vgm"
#define PSG_ADDR 0xC00011

void interrupt timerHandler(void);

unsigned char MUSIC_load();
void MUSIC_free();
void MUSIC_start();
void MUSIC_stop();
