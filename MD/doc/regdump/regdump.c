// #include <dos.h>
#include<stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <i86.h>
#include <bios.h>

// VGA Ports and register counts. Taken from 
// https://github.com/jedld/dex-os/blob/master/hardware/vga/dexvga.c
// Verified against TeraDrive VGA chip datasheet, WD90C10.
#define	VGA_AC_INDEX		0x3C0
#define	VGA_AC_WRITE		0x3C0
#define	VGA_AC_READ		    0x3C1
#define	VGA_MISC_WRITE      0x3C2
#define VGA_SEQ_INDEX		0x3C4
#define VGA_SEQ_DATA		0x3C5
#define	VGA_DAC_READ_INDEX	0x3C7
#define	VGA_DAC_WRITE_INDEX	0x3C8
#define	VGA_DAC_DATA		0x3C9
#define	VGA_MISC_READ		0x3CC
#define VGA_GC_INDEX 		0x3CE
#define VGA_GC_DATA 		0x3CF
#define VGA_CRTC_INDEX	    0x3D4
#define VGA_CRTC_DATA		0x3D5
#define	VGA_INSTAT_READ	    0x3DA

#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21
#define	VGA_NUM_REGS		(1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + \
				VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)
int i;
int j;
char this;
unsigned char regs[VGA_NUM_REGS];
char filename[12];
const char* prefix = "MODE";
const char* suffix = ".TXT";
const char modes[] = {0x13, 0x01, 0x03};
const char *modenames[3] = {"13H","01H","03H"};

int main(int argc, char *argv[]) 
{
  union REGS inregs, outregs;

  for(j = 0; j < 3; j++){

    inregs.h.ah = 0;
    inregs.h.al = modes[j];
    int86(0x10, &inregs, &outregs);
    
    delay(1000);

    read(regs);

    sprintf(filename,"%s%s%s",prefix,modenames[j],suffix);
    
    printregs(regs);

  }

  printf("Done.");

  return 0;
}

void printregs(unsigned char *regs)
{
  
  FILE *f;
  f = fopen(filename,"w");
  printf("\nMISC\n");
  fwrite("MISC\n",5,1,f);
  printf("0x3C2\t%02x", *regs);
  fprintf(f,"0x3C2\t%02x", *regs);
  
  regs++;
  printf("\n");
  printf("\nSEQ\n");
  fprintf(f,"\n\nSEQ\n");
  for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		printf("%02x\t%02x\n",i,*regs);
    fprintf(f,"%02x\t%02x\n",i,*regs);
		regs++;
	}

  printf("\nCRTC\n");
  fprintf(f,"\nCRTC\n");
  for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		printf("%02x\t%02x\n",i,*regs);
		fprintf(f,"%02x\t%02x\n",i,*regs);
		regs++;
	}

  printf("\nGC\n");
  fprintf(f,"\nGC\n");
  for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		printf("%02x\t%02x\n",i,*regs);
		fprintf(f,"%02x\t%02x\n",i,*regs);
		regs++;
	}

  printf("\nAC\n");
  fprintf(f,"\nAC\n");
  for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		printf("%02x\t%02x\n",i,*regs);
		fprintf(f,"%02x\t%02x\n",i,*regs);
		regs++;
	}

  fclose(f);
}

void read(unsigned char *regs)
{
/* read MISCELLANEOUS reg */
	*regs = inp(VGA_MISC_READ);
	regs++;
/* read SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		outp(VGA_SEQ_INDEX, i);
		*regs = inp(VGA_SEQ_DATA);
		regs++;
	}
/* read CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outp(VGA_CRTC_INDEX, i);
		*regs = inp(VGA_CRTC_DATA);
		regs++;
	}
/* read GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outp(VGA_GC_INDEX, i);
		*regs = inp(VGA_GC_DATA);
		regs++;
	}
/* read ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		(void)inp(VGA_INSTAT_READ);
		outp(VGA_AC_INDEX, i);
		*regs = inp(VGA_AC_READ);
		regs++;
	}
/* lock 16-color palette and unblank display */
	(void)inp(VGA_INSTAT_READ);
	outp(VGA_AC_INDEX, 0x20);

}
