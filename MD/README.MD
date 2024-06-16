# MD-side Demo for Sega Teradrive

## What it does
* Detects real TeraDrives
* Shows some info text and a sprite
  * Gets real time from PC side RTC
  * Read BIOS string
  * Shows system state
* (Optional) If you have a PC POST analyser card in the ISA slot, it displays the frame count. In decimal!!
* Using the controller, select between the RGB output showing the VGA or VDP output, and whether the VGA is showing graphics, 80col text or 40col text.

## How to build it
* Install VS Code
* Install and configure SGDK (needs Java)
* Install **Genesis Code** VS Code extension
* Build
