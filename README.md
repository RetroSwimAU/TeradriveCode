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
  * I have written a small demo that puts some text on the Mega Drive's display using the VDP, and some text on the PC's display using VGA, and lets you toggle between them by pushing B and C on the controller.
    * VGA RAM begins at `B800:0000`, default 80col text mode uses 1 attribute byte and 1 character byte per displayed character.
    * A beep also plays thru the PC speaker, by writing to IO ports `0x42 0x43 and 0x61` (`$AF0042, $AF0043, $AF0061` respestively in M68K space)
* Accessing MD resources from PC
  * The system must first be "unlocked". See above forum post, or `main.c` for what this entails.
    * tl;dr Sets some of the M68K registers with port writes to `0x1160 - 0x1167`
  * ~~The forum post isn't clear on where the "PRODUCED BY..." text really needs to be. Initially, it says the M68K searches in PC ROM space at `C000:0000`, but then says it can at any even address in PC conventional memory (i.e. word boundaries).~~ When the registers `0x1166` and `0x1167` are both set to 0, this is a special case directing the M68K to search the PC conventional memory area `0000:0000 - A000:0000` for the "PRODUCED BY..." text. Details in the forum post above. I rely on the fact that the C compiler put the string into the data segment on a word boundary. When I check the pointer value it's always even. I guess it's like that to facilitate faster 16 bit reads. I'm no C wizard lol. 
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
  * There is a 'CMOS Setup' type screen accessible by booting the TeraDrive in PC mode, choosing DOS from the boot menu, and spamming F1. On this screen you can select which PC segment the memory window appears in by default. The base address can be from `C800:0000` to `DE00:0000`. The default is `CE00:0000`
    * The top two bytes are visible in register 1162. E.g. the default value is `0xCE`. This register can be updated by either CPU at runtime though.
  * This is how I access the PSG memory location for my demo.
    * The calculations are easier though, because the PSG address is `$C00011`. All the middle bits are 0's :D
    * The playback of the Master System music is, well, barbaric. The file is read a byte at a time, sending the PSG commands as it finds them, ignoring any wait/delay commands, as fast as it can. The fact I update the file position on the screen just happens to make it play sorta-kinda the right speed.
    

## Access MD resources from PC side
![image](https://github.com/RetroSwimAU/TeradriveCode/assets/45222648/e39d36f9-ca98-44e3-af8f-1b0ad07a6c7e)


Run `pwsh build.ps1` in the project directory. This is from [a generous githubber](https://github.com/pleft/SEGA_VSCode_Template). Thank you!

### Issues
* ~~Sometimes hangs on `outp(0x1164, 0x81);`. I think the M68k isn't coming out of reset. I'm not sure why.~~
  * This was related to the PicoMEM I have in my TeraDrive. Moved its boot ROM to D000 and problem solved.
* ~~The VGM playback is just reading the file as fast as it can, one byte at a time. That ain't it chief. It was just a way to get it going with my non-existent C and DOS programming knowledge.~~
  * ~~Now uses `fread()` which is significantly faster than `_dos_read()`, so song now plays at recognisable pace! Still not a nice constant pace, but 'good enough' to prove the concept.~~
  * Now uses interrupt driven playback, however since it's just a 10MHz 286, I had to make some compromises to make it play fast AND work with all the other stuff going on. Playback is 99% awesome, just a few minor spots it has a sad.
* ~~The VGM playback is tailored to this specific VGM file, and its version of the VGM header. To use a different one, replace `test.vgm`.~~
  * ~~If your VGM file is compressed (.vgm.gz or .vgz extension), `gunzip` it first.~~
  * ~~If your VGM file's header is not 0x40 bytes long, I can't vouch for the results.~~
  * ~~If your VGM file uses commands other than 0x4f 0x50 0x61-0x63 0x66, I can't vouch for the results.~~
  * Don't change the VGM file ok? :D
* Shows something nice on the PC side now!! There's more to the message than you see at first! ;)
* Has controls!! Up/Down on the Dpad to scroll, A/B to switch outputs.
* Has keyboard! Space to toggle outputs. Escape to quit.
* Shows stuff on the MD VDP! A static message but hey!
* Scrolling has minor issues:
  * In "Video" mode, it jumps vertically every 16 pixels, I think I need to calculate new values for manipulating the line compare register.
  * In "RGB" mode it flickers very occasionally, I think if a music interrupt happens near after V retrace finishes but before address register changes.
  * Still lookg pretty nice IMO, especially for a first try, and with so many variables.
 
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

![image](https://github.com/RetroSwimAU/TeradriveCode/assets/45222648/d5289727-6aba-45ec-a08a-7bf9caca01d0)

As of 0.4 this is now the more sophisticated of the two demos.

To build, you need SGDK installed and configured. I used the VSCode extension Genesis Code. With those set up, open the MD folder in VSCode and hit build.

### Features:
* Detects if running on an emulator or normal Mega Drive/Genesis. Try it! :)
* Draws text to the VDP, showing the PC'S RTC, frame counter, and some status info and instructions. As well as a fun sprite and some music. This done using SGDK routines, all very standard. Just to show the MD hardware is being used.
* Shows text and a frame counter on the VGA screen, in either 40 col or 80 col mode. Press B for 80 columns, C for 40 columns.
  * This respects the RGB/Video switch on the back of the system, and uses the 15kHz variants of VGA modes as appropriate.
  * Backs up VRAM and DAC registers to PC conventional RAM when switching away from text mode (and restores when leaving graphics mode), since x86 interrupts are unavailable to the M68K, mode switches are done with pure register writes. This was a simple method to to fix font and palette corruption.
* Pushing A shows a graphical screen in mode 13h, with palette cycling.
* Pushing up or down switches the output between VGA and MD VDP.
  * When the rear switch is set to "Video", the composite output follows the toggle, and VGA uses 15kHz modes.
  * When set to "RGB", the composite output always stays on VDP, and the PC uses 31kHz modes.
* Writes frame counter to PC IO port `0x80`, converted to BCD, so hex display on POST analyser card shows "decimal" frame count! 
* Greets and credits!

Sample: https://youtu.be/o8Qj7M0l1RY

### Resources

* SGDK - https://github.com/Stephane-D/SGDK
* VGA without int 10h - https://github.com/jedld/dex-os/blob/master/hardware/vga/dexvga.c
* VGA register programming - http://www.osdever.net/FreeVGA/vga/vga.htm
  
