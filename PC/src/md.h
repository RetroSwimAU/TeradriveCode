#define VDP_CONTROL 0xC00004
#define VDP_DATA 0xC00000

void MD_updateWindow(long addr);
void MD_setBase(unsigned char base);
void MD_memoryWriteB(unsigned long addr, unsigned char data);
void MD_memoryWriteW(unsigned long addr, unsigned int data);
int MD_setup();
unsigned char MD_isRGB();
