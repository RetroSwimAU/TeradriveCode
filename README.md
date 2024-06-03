# My TeraDrive Experiments

# MD PSG playback from PC side
Should just need to clone and `wmake` in this directory.

## Issues
* Sometimes hangs on `outp(0x1164, 0x81);`. I think the M68k isn't coming out of reset. I'm not sure why.
* The VGM playback is just reading the file as fast as it can, one byte at a time. That ain't it chief. It was just a way to get it going with my non-existent C and DOS programming knowledge.
* The VGM playback is tailored to this specific VGM file, and its version of the VGM header. To use a different one, replace `test.vgm`.
  * If your VGM file is compressed (.vgm.gz or .vgz extension), `gunzip` it first.
  * If your VGM file's header is not 0x40 bytes long, I can't vouch for the results.
  * If your VGM file uses commands other than 0x4f 0x50 0x61-0x63 0x66, I can't vouch for the results.
 
## Resources
* Info on the 'startup sequence' comes from this forum post from 2018. I believe this is based on reverse-engineering of Puzzle Construction Kit.
  * https://gendev.spritesmind.net/forum/viewtopic.php?t=2887
  * Credit: Mask of Destiny, Nemesis and piroxilin
* Info on PSG programming comes from this concise and informative M68k ASM tutorial site
  * https://www.hacking-cult.org/
  * Credit: drx
* Built with OpenWatcom 2.0
  * https://github.com/open-watcom/open-watcom-v2/releases/tag/2024-06-01-Build

# PC IO writes from MD side
Download MD Studio, open `hello_world_with_PC_IO_write.s`, and build. Put resulting BIN file on EverDrive/etc, boot on TeraDrive in MD mode.

## Issues
* Needs an ISA POST analyser card listening on port 0x80 installed.

## Resources
* Same forum post as above
* MD Studio was used to build 68K ASM into a bootable Mega Drive BIN file
  * https://github.com/gouky/MDStudio
  * Credit: gouky
* Source is from https://github.com/BigEvilCorporation/megadrive_samples but with my line added.
