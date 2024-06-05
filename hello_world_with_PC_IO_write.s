; Matt Phillips - bigevilcorporation.co.uk

ROM_Start

	; CPU vector table
	dc.l   0x00FFE000			; Initial stack pointer value
	dc.l   CPU_EntryPoint		; Start of program
	dc.l   CPU_Exception 		; Bus error
	dc.l   CPU_Exception 		; Address error
	dc.l   CPU_Exception 		; Illegal instruction
	dc.l   CPU_Exception 		; Division by zero
	dc.l   CPU_Exception 		; CHK CPU_Exception
	dc.l   CPU_Exception 		; TRAPV CPU_Exception
	dc.l   CPU_Exception 		; Privilege violation
	dc.l   INT_Null				; TRACE exception
	dc.l   INT_Null				; Line-A emulator
	dc.l   INT_Null				; Line-F emulator
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Spurious exception
	dc.l   INT_Null				; IRQ level 1
	dc.l   INT_Null				; IRQ level 2
	dc.l   INT_Null				; IRQ level 3
	dc.l   INT_HBlank			; IRQ level 4 (horizontal retrace interrupt)
	dc.l   INT_Null  			; IRQ level 5
	dc.l   INT_VBlank			; IRQ level 6 (vertical retrace interrupt)
	dc.l   INT_Null				; IRQ level 7
	dc.l   INT_Null				; TRAP #00 exception
	dc.l   INT_Null				; TRAP #01 exception
	dc.l   INT_Null				; TRAP #02 exception
	dc.l   INT_Null				; TRAP #03 exception
	dc.l   INT_Null				; TRAP #04 exception
	dc.l   INT_Null				; TRAP #05 exception
	dc.l   INT_Null				; TRAP #06 exception
	dc.l   INT_Null				; TRAP #07 exception
	dc.l   INT_Null				; TRAP #08 exception
	dc.l   INT_Null				; TRAP #09 exception
	dc.l   INT_Null				; TRAP #10 exception
	dc.l   INT_Null				; TRAP #11 exception
	dc.l   INT_Null				; TRAP #12 exception
	dc.l   INT_Null				; TRAP #13 exception
	dc.l   INT_Null				; TRAP #14 exception
	dc.l   INT_Null				; TRAP #15 exception
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)
	dc.l   INT_Null				; Unused (reserved)

	; ROM metadata
	dc.b "SEGA MEGA DRIVE "                                 ; Console name
	dc.b "AUTHOR NAME HERE"                                 ; Copyright holder and release date
	dc.b "GAME TITLE                                      " ; Domestic name
	dc.b "GAME TITLE                                      " ; International name
	dc.b "GM XXXXXXXX-XX"                                   ; Version number
	dc.w 0x0000                                             ; Checksum
	dc.b "J               "                                 ; I/O support
	dc.l ROM_Start                                          ; Start address of ROM
	dc.l ROM_End-1                                          ; End address of ROM
	dc.l 0x00FF0000                                         ; Start address of RAM
	dc.l 0x00FF0000+0x0000FFFF                              ; End address of RAM
	dc.l 0x00000000                                         ; SRAM enabled
	dc.l 0x00000000                                         ; Unused
	dc.l 0x00000000                                         ; Start address of SRAM
	dc.l 0x00000000                                         ; End address of SRAM
	dc.l 0x00000000                                         ; Unused
	dc.l 0x00000000                                         ; Unused
	dc.b "                                        "         ; Notes (unused)
	dc.b "  E             "                                 ; Country codes
	
; Initial VDP register values
VDPRegisters:
	dc.b 0x14 ; 0:  H interrupt on, palettes on
	dc.b 0x74 ; 1:  V interrupt on, display on, DMA on, Genesis mode on
	dc.b 0x30 ; 2:  Pattern table for Scroll Plane A at VRAM 0xC000 (bits 3-5 = bits 13-15)
	dc.b 0x00 ; 3:  Pattern table for Window Plane at VRAM 0x0000 (disabled) (bits 1-5 = bits 11-15)
	dc.b 0x07 ; 4:  Pattern table for Scroll Plane B at VRAM 0xE000 (bits 0-2 = bits 11-15)
	dc.b 0x78 ; 5:  Sprite table at VRAM 0xF000 (bits 0-6 = bits 9-15)
	dc.b 0x00 ; 6:  Unused
	dc.b 0x00 ; 7:  Background colour - bits 0-3 = colour, bits 4-5 = palette
	dc.b 0x00 ; 8:  Unused
	dc.b 0x00 ; 9:  Unused
	dc.b 0x08 ; 10: Frequency of Horiz. interrupt in Rasters (number of lines travelled by the beam)
	dc.b 0x00 ; 11: External interrupts off, V scroll fullscreen, H scroll fullscreen
	dc.b 0x81 ; 12: Shadows and highlights off, interlace off, H40 mode (320 x 224 screen res)
	dc.b 0x3F ; 13: Horiz. scroll table at VRAM 0xFC00 (bits 0-5)
	dc.b 0x00 ; 14: Unused
	dc.b 0x02 ; 15: Autoincrement 2 bytes
	dc.b 0x01 ; 16: Vert. scroll 32, Horiz. scroll 64
	dc.b 0x00 ; 17: Window Plane X pos 0 left (pos in bits 0-4, left/right in bit 7)
	dc.b 0x00 ; 18: Window Plane Y pos 0 up (pos in bits 0-4, up/down in bit 7)
	dc.b 0xFF ; 19: DMA length lo byte
	dc.b 0xFF ; 20: DMA length hi byte
	dc.b 0x00 ; 21: DMA source address lo byte
	dc.b 0x00 ; 22: DMA source address mid byte
	dc.b 0x80 ; 23: DMA source address hi byte, memory-to-VRAM mode (bits 6-7)
	
	even
	
; VDP port addresses
vdp_control				equ 0x00C00004
vdp_data				equ 0x00C00000

; VDP commands
vdp_cmd_vram_write		equ 0x40000000
vdp_cmd_cram_write		equ 0xC0000000

; Hardware version address
hardware_ver_address	equ 0x00A10001

; TMSS
tmss_address			equ 0x00A14000
tmss_signature			equ 'SEGA'
	
; Data port setup macros
SetVRAMWrite: macro addr
	move.l  #(vdp_cmd_vram_write)|((\addr)&$3FFF)<<16|(\addr)>>14, vdp_control
    endm
	
SetCRAMWrite: macro addr
	move.l  #(vdp_cmd_cram_write)|((\addr)&$3FFF)<<16|(\addr)>>14, vdp_control
    endm
    
StatusWrite: macro val
	move.b #(\val),0x00AF0080
	endm
	
	; Palette
Palette:
	dc.w 0x0000	; Transparent
	dc.w 0x0000	; Black
	dc.w 0x0EEE	; White
	dc.w 0x000E	; Red
	dc.w 0x00E0	; Blue
	dc.w 0x0E00	; Green
	dc.w 0x0E0E	; Pink
	dc.w 0x0000
	dc.w 0x0000
	dc.w 0x0000
	dc.w 0x0000
	dc.w 0x0000
	dc.w 0x0000
	dc.w 0x0000
	dc.w 0x0000
	dc.w 0x0000
	
	; Font glyphs for "HELOWRD"
CharacterH:
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22222220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x00000000
	
CharacterE:
	dc.l 0x22222220
	dc.l 0x22000000
	dc.l 0x22000000
	dc.l 0x22222220
	dc.l 0x22000000
	dc.l 0x22000000
	dc.l 0x22222220
	dc.l 0x00000000
	
CharacterL:
	dc.l 0x22000000
	dc.l 0x22000000
	dc.l 0x22000000
	dc.l 0x22000000
	dc.l 0x22000000
	dc.l 0x22000000
	dc.l 0x22222220
	dc.l 0x00000000
	
CharacterO:
	dc.l 0x22222220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22222220
	dc.l 0x00000000
	
CharacterW:
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22020220
	dc.l 0x22020220
	dc.l 0x22020220
	dc.l 0x22222220
	dc.l 0x00000000
	
CharacterR:
	dc.l 0x22222200
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22222200
	dc.l 0x22022000
	dc.l 0x22002200
	dc.l 0x22000220
	dc.l 0x00000000
	
CharacterD:
	dc.l 0x22222200
	dc.l 0x22002220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22000220
	dc.l 0x22002220
	dc.l 0x22222200
	dc.l 0x00000000

CPU_EntryPoint

	jsr VDP_WriteTMSS
	jsr VDP_LoadRegisters
	
	SetCRAMWrite 0x0000
	move.l #Palette, a0
	move.w #0xF, d0			; Loop counter = 16 words in palette (-1)
	@PalLp:					; Start of loop
	move.w (a0)+, vdp_data	; Write palette entry, post-increment address
	dbra d0, @PalLp			; Loop until finished
	
	SetVRAMWrite 0x0020
	move.l #CharacterH, a0
	move.w #(8*40)-1, d0	; Loop counter = 8 longwords in tile (-1)
	@CharLp:				; Start of loop
	move.l (a0)+, vdp_data	; Write tile line, post-increment address
	dbra d0, @CharLp		; Loop until finished
	
	SetVRAMWrite 0xC418
	move.w #0x0001, vdp_data	; H
	SetVRAMWrite 0xC41A
	move.w #0x0002, vdp_data	; E
	SetVRAMWrite 0xC41C
	move.w #0x0003, vdp_data	; L
	SetVRAMWrite 0xC41E
	move.w #0x0003, vdp_data	; L
	SetVRAMWrite 0xC420
	move.w #0x0004, vdp_data	; 0
	
	SetVRAMWrite 0xC424
	move.w #0x0005, vdp_data	; W
	SetVRAMWrite 0xC426
	move.w #0x0004, vdp_data	; O
	SetVRAMWrite 0xC428
	move.w #0x0006, vdp_data	; R
	SetVRAMWrite 0xC42A
	move.w #0x0003, vdp_data	; L
	SetVRAMWrite 0xC42C
	move.w #0x0007, vdp_data	; D
	
	; Write 0xAA to PC IO port 0x80 - value will display on POST card
	StatusWrite 0xAA
	move.b #0x83,0x00AF1164
	StatusWrite 0xBB
	
	move.w #0x579E, 0x00BB8000
	move.w #0x729E, 0x00BB8002
	move.w #0x699E, 0x00BB8004
	move.w #0x749E, 0x00BB8006
	move.w #0x699E, 0x00BB8008
	move.w #0x6E9E, 0x00BB800A
	move.w #0x679E, 0x00BB800C
	move.w #0x209E, 0x00BB800E
	move.w #0x569E, 0x00BB8010
	move.w #0x479E, 0x00BB8012
	move.w #0x419E, 0x00BB8014
	move.w #0x209E, 0x00BB8016
	move.w #0x549E, 0x00BB8018
	move.w #0x459E, 0x00BB801A
	move.w #0x589E, 0x00BB801C
	move.w #0x549E, 0x00BB801E
	move.w #0x209E, 0x00BB8020
	move.w #0x669E, 0x00BB8022
	move.w #0x729E, 0x00BB8024
	move.w #0x6F9E, 0x00BB8026
	move.w #0x6D9E, 0x00BB8028
	move.w #0x209E, 0x00BB802A
	move.w #0x4D9E, 0x00BB802C
	move.w #0x369E, 0x00BB802E
	move.w #0x389E, 0x00BB8030
	move.w #0x4B9E, 0x00BB8032
	move.w #0x209E, 0x00BB8034
	move.w #0x6F9E, 0x00BB8036
	move.w #0x6E9E, 0x00BB8038
	move.w #0x209E, 0x00BB803A
	move.w #0x549E, 0x00BB803C
	move.w #0x659E, 0x00BB803E
	move.w #0x729E, 0x00BB8040
	move.w #0x619E, 0x00BB8042
	move.w #0x449E, 0x00BB8044
	move.w #0x729E, 0x00BB8046
	move.w #0x699E, 0x00BB8048
	move.w #0x769E, 0x00BB804A
	move.w #0x659E, 0x00BB804C
	
	StatusWrite 0xCC
	
	; Begin reading controller
  CtrlLoop:
	moveq	#$40,d0
	move.b	d0,($A10009).l	; TH pin to write, others to read
	move.b	d0,($A10003).l	; TH to 1
	nop
	nop
	move.b	($A10003).l,d0
	andi.b	#$3F,d0		; d0 = 00CBRLDU
	moveq	#0,d1
	move.b	#0,($A10003).l	; TH to 0
	nop
	nop
	move.b	($A10003).l,d1
	andi.b	#$30,d1		; d1 = 00SA0000
	lsl.b	#2,d1		; d1 = SA000000
	or.b	d1,d0		; d0 = SACBRLDU
	
	move.b  #$20,d2 ; C test
	and.b	d0, d2
	tst.b   d2
	beq.b   CPress
	
	move.b  #$10,d2 ; B test
	and.b	d0, d2
	tst.b   d2
	beq.b   BPress
	
	jmp CtrlLoop
	
  CPress:
    
    move.b #0x83,0x00AF1164
    jsr Beep
    jmp CtrlLoop
	
  BPress:
  
    move.b #0x87,0x00AF1164
    jsr Beep
    jmp CtrlLoop
    
  Beep:
  
  	move.b 0x00AF0061, d3
  	move.b d3, d4
  	ori.b #0x03, d4
  	move.b #0xB6,0x00AF0043
  	move.b #0xD0,0x00AF0042
  	move.b #0x11,0x00AF0042
  	move.b d4, 0x00AF0061
    move.w #0xFFFF, d5
  BeepDelay:
    subi.w #0x0001, d5
  	tst.w d5
  	bne.w BeepDelay
  	move.b d3, 0x00AF0061
  	rts
	
INT_VBlank:
	rte

INT_HBlank:
	rte

INT_Null:
	rte

CPU_Exception:
	stop   #0x2700
	rte
	
VDP_WriteTMSS:

	move.b hardware_ver_address, d0			; Move Megadrive hardware version to d0
	andi.b #0x0F, d0						; The version is stored in last four bits, so mask it with 0F
	beq @Skip								; If version is equal to 0, skip TMSS signature
	move.l #tmss_signature, tmss_address	; Move the string "SEGA" to 0xA14000
	@Skip:

	; Check VDP
	move.w vdp_control, d0					; Read VDP status register (hangs if no access)
	
	rts
	
VDP_LoadRegisters:

	; Set VDP registers
	move.l #VDPRegisters, a0	; Load address of register init table into a0
	move.w #0x17, d0			; 24 registers to write (-1 for loop counter)
	move.w #0x8000, d1			; 'Set register 0' command

	@CopyVDP:
	move.b (a0)+, d1			; Move register value to lower byte of d1
	move.w d1, vdp_control		; Write command and value to VDP control port
	add.w  #0x0100, d1			; Increment register #
	dbra   d0, @CopyVDP
	
	rts
	
ROM_End
