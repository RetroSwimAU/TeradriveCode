# Documentation
I had a real rough time getting the VGA mode switching to work. Turns out the TeraDrive's VGA BIOS is a bit special. Because of course it is.

These are dumps of the VGA registers for mode 13h (320x200x8 indexed), and modes 01h/03h (40/80 col text respectively), after using the INT 10h routines in DOS, in both RGB and Video mode. When in video mode, the TeraDrive uses modes that are both 15kHz and VGA-memory-architecture compatible. In RGB mode, it behaves like a normal PC.

I've also attached the source for the little app I wrote to create the dumps.
