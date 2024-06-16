#define PC_MEMORY_BASE 0xB00000
#define PC_IO_BASE     0xAF0000

// // Registers and values for PC mode 13h switch. The following is in roughly chronological order.
// #define PC_13H_3C2_CLOCKSELECT 0x63

// #define PC_13H_3D4_UNLOCK_REGS 0x0E11

// #define PC_13H_3D4_COUNT_HOR 7
// #define PC_13H_3D4_COUNT_VER 8
// #define PC_13H_3D4_PANNING_OFF 0x0008

// #define PC_13H_3D4_COUNT_CHAIN4 3
// #define PC_13H_3C4_COUNT_PLANES 2
// #define PC_13H_3CE_COUNT_COLOR_GRAPH 2
// #define PC_13H_3C0_ENABLE_VIDEO 0x20

// ** TERADRIVE SPECIFIC REGISTERS**
// Function unclear
#define TD_SHARED_REGISTER_0 0xAF1160

// Function unclear
#define TD_SHARED_REGISTER_1 0xAF1161

// Base address for PC's window to MD RAM.
// 0xCE by default. Only bits 1-4 can be changed.
#define TD_SHARED_REGISTER_2 0xAF1162


#define TD_SHARED_REGISTER_3 0xAF1163
#define TD_SR3_ENABLE_MEMORY_WINDOW 0x000002
#define TD_SR3_AE0001_0 0x000010
#define TD_SR3_AE0001_1 0x000020

// For M68K software, bit 2 is most important.
#define TD_SHARED_REGISTER_4 0xAF1164
#define TD_SR4_CPU_SWITCH 0x000001
#define TD_SR4_CART_AT_ZERO 0x000002
#define TD_SR4_VIDEO_SOURCE 0x000004
#define TD_SR4_TEST_MODE 0x000010
#define TD_SR4_UNCLEAR_6 0x000040
#define TD_SR4_UNCLEAR_7 0x000080

// Some system status bits
#define TD_SHARED_REGISTER_5 0xAF1165
#define TD_SR5_MODE_SWITCH 0x000001
#define TD_SR5_VIDEO_SWITCH 0x000004
#define TD_SR5_UNCLEAR_3 0x000008
#define TD_SR5_MD_HARDWARE_UNLOCKED 0x000020

// Address of M68K memory visible to PC
#define TD_SHARED_REGISTER_6 0xAF1166
#define TD_SHARED_REGISTER_7 0xAF1167


void PC_memoryWriteFlatB(int address, char value);

void PC_memoryWriteB(u16 segment, u16 offset, char value);

char PC_memoryReadFlatB(int address);

char PC_memoryReadB(u16 segment, u16 offset);

void PC_IOPortWriteB(int port, char value);

void PC_IOPortWriteW(int port, u16 value);

char PC_IOPortReadB(int port);

void PC_drawVGAText(char * str, int len, u8 x, u8 y);

void PC_goVGAText80(u8 isRGB);

void PC_goVGAText40(u8 isRGB);

void PC_goVGA13h(u8 isRGB);

void TD_exposeMemory(int address);

void TD_setOutputVDP();

void TD_setOutputVGA();
