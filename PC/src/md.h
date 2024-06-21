#define VDP_CONTROL 0xC00004
#define VDP_DATA 0xC00000
#define VDP_SETUP_COUNT 24
#define VDP_ENABLE_SCREEN 0x8144

#define VDP_COLOUR_0_REG  0xC0000000
#define VDP_COLOUR_1_REG  0xC0020000
#define VDP_COLOUR_2_REG  0xC0040000
#define VDP_COLOUR_3_REG  0xC0060000
#define VDP_COLOUR_15_REG 0xC01E0000
//                      -BGR
#define VDP_COLOUR_0  0x0300
#define VDP_COLOUR_1  0x0066
#define VDP_COLOUR_2  0x0660
#define VDP_COLOUR_3  0x0006
#define VDP_COLOUR_15 0x0066

#define VDP_VRAM 0x40000000

#define FONT_FILE "font.bin"

void MD_init();
void MD_boot68K();
void MD_enableWindow();
void MD_updateWindow(long addr);
void MD_setBase(unsigned char base);
void MD_memoryWriteB(unsigned long addr, unsigned char data);
void MD_memoryWriteW(unsigned long addr, unsigned int data);
void MD_memoryWriteL(unsigned long addr, unsigned long data);
int MD_setup();
unsigned char MD_isRGB();
void MD_startVDP();
unsigned char MD_loadfont();
