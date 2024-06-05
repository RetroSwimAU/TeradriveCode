# My TeraDrive Experiments

## How it works

This all comes from https://gendev.spritesmind.net/forum/viewtopic.php?t=2887 ... but here is my attempt at explaining what's happening here.

* Unlike the Amstrad MegaPC, which is a PC with a MegaDrive on a card, sharing only the chassis, power supply, and some external connectors, the Sega TeraDrive is an integrated system
* There is some custom silicon gluing the sides together. The PC's IO and memory space have a bidirectional link to the MD's memory space. I have no further detail on this.
* The 286 and M68K don't run at the same time. While one executes code, the other is held in reset.
  * Nemesis proposes there is a way to run the two CPUs simultaneously, see forum post above. I haven't verified this, but could make dev/debugging possible?
* Accessing PC resources from MD
  * While executing MegaDrive software, the PC's memory under 1MB is visible at `$B00000 - $BFFFFF`. This is space marked "Reserved" in most MD development resources.
    * Uses 20-bit notation, convert segmented addresses like `(segment << 4) + offset`
  * While executing MegaDrive software, the PC's IO space can be read/written by accessing memory at `$AF0000 - $AFFFFF`. This is space marked "Reserved" in most MD development resources.
  * No action is required to 'activate' this functionality, from what I can tell. My test below writes to $AF0080 without any specific prep and it 'just works'.
  * Since the 286 CPU is normally halted while M68K runs code, you could feasibly use the PC's RAM as an extra megabyte-ish for MD software (excluding ROM BIOS region etc)???
  * Setting $AE0003 in M68K RAM lets you select a "bank" of 1MB from the PC side to view. However TeraDrive only has 2.5MB of RAM tops, and no MMIO to speak of. So probably of limited usefulness.
  * All I've done with this is write a byte to 0x80 for display on a POST card. Could make the PC speaker beep or something I guess!
* Accessing MD resources from PC
  * The system must first be "unlocked". See above forum post, or `main.c` for what this entails.
    * tl;dr Sets some of the M68K registers with port writes to `0x1160 - 0x1167`
  * The forum post isn't clear on where the "PRODUCED BY..." text really needs to be. Initially, it says the M68K searches in PC ROM space at `C000:0000`, but then says it can at any even address in PC conventional memory (i.e. word boundaries). I rely on the fact that the C compiler put the string into the data segment on a word boundary. When I check the pointer value it's always even. I guess it's like that to facilitate 16 bit reads. I'm no C wizard lol.
  * With the system "unlocked", the MegaDrive's memory can be read+written by the 286 thru an 8K window. The default window is at `CE00:0000 - CE00:1FFF`. For instance, to read MD address `$5A5A5A`:
    * Calculate bits:
      * M68K "base address high" bits 20-23 = `(0x5A5A5A & 0xF00000) >> 20 = 0x05`
      * M68K "base address low" bits 13-19 = `(0x5A5A5A & 0x0FE000) >> 12 = 0xA4`
      * M68K "offset" bits 0-12 = `(0x5A5A5A & 0x001FFF) = 0x1A5A`
    * Port writes:
      * Port `0x1167`, write `0x05`
      * Port `0x1166`, write `0xA4`
    * Then finally:
      * PC address `CE00:1A5A`
  * This is how I access the PSG memory location for my demo.
    * The calculations are easier though, because the PSG address is `$C00011`. All the middle bits are 0's :D
    * The playback of the Master System music is, well, barbaric. The file is read a byte at a time, sending the PSG commands as it finds them, ignoring any wait/delay commands, as fast as it can. The fact I update the file position on the screen just happens to make it play sorta-kinda the right speed.

## MD PSG playback from PC side
![image](https://github.com/RetroSwimAU/TeradriveCode/assets/45222648/87bc9323-5314-4551-88e6-4b3b46e08b6c)

Should just need to clone and `wmake` in this directory.

### Issues
* ~~Sometimes hangs on `outp(0x1164, 0x81);`. I think the M68k isn't coming out of reset. I'm not sure why.~~
  * This was related to the PicoMEM I have in my TeraDrive. Moved its boot ROM to D000 and problem solved.
* The VGM playback is just reading the file as fast as it can, one byte at a time. That ain't it chief. It was just a way to get it going with my non-existent C and DOS programming knowledge.
  * Now uses `fread()` which is significantly faster than `_dos_read()`, so song now plays at recognisable pace! Still not a nice constant pace, but 'good enough' to prove the concept.
* The VGM playback is tailored to this specific VGM file, and its version of the VGM header. To use a different one, replace `test.vgm`.
  * If your VGM file is compressed (.vgm.gz or .vgz extension), `gunzip` it first.
  * If your VGM file's header is not 0x40 bytes long, I can't vouch for the results.
  * If your VGM file uses commands other than 0x4f 0x50 0x61-0x63 0x66, I can't vouch for the results.
 
### Resources
* Info on the 'startup sequence' comes from this forum post from 2018. I believe this is based on reverse-engineering of Puzzle Construction Kit.
  * https://gendev.spritesmind.net/forum/viewtopic.php?t=2887
  * Credit: Mask of Destiny, Nemesis and piroxilin
* Info on PSG programming comes from this concise and informative M68k ASM tutorial site
  * https://www.hacking-cult.org/
  * Credit: drx
* Built with OpenWatcom 2.0
  * https://github.com/open-watcom/open-watcom-v2/releases/tag/2024-06-01-Build

## PC IO writes from MD side
![GPITWSba8AAq1g-](https://github.com/RetroSwimAU/TeradriveCode/assets/45222648/ab0012ab-d667-40b1-ba22-a88d5359980b)

Download MD Studio, open `hello_world_with_PC_IO_write.s`, and build. Put resulting BIN file on EverDrive/etc, boot on TeraDrive in MD mode.

These instructions are equivalent on the TeraDrive.
* 68K: `move.b #0xAA,0x00AF0080`
* PC: `mov al, AAh; out 80h, al;`
  
### Issues
* Needs an ISA POST analyser card listening on port 0x80 installed.

### Resources
* Same forum post as above
* MD Studio was used to build 68K ASM into a bootable Mega Drive BIN file
  * https://github.com/gouky/MDStudio
  * Credit: gouky
* Source is from https://github.com/BigEvilCorporation/megadrive_samples but with my line added.
