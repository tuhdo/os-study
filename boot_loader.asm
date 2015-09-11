;*********************************************
;	Boot1.asm
;		- A Simple Bootloader
;
;	Operating Systems Development Tutorial
;*********************************************

bits	16							; We are still in 16 bit Real Mode

org		0x7c00						; We are loaded by BIOS at 0x7C00

start:          jmp loader					; jump over OEM block

;*************************************************;
;	OEM Parameter block
;*************************************************;

; Error Fix 2 - Removing the ugly TIMES directive -------------------------------------

;;	TIMES 0Bh-$+start DB 0					; The OEM Parameter Block is exactally 3 bytes
								; from where we are loaded at. This fills in those
								; 3 bytes, along with 8 more. Why?

bpbOEM			db "My OS   "				; This member must be exactally 8 bytes. It is just
								; the name of your OS :) Everything else remains the same.

bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	    DB 2
bpbRootEntries: 	    DW 224
bpbTotalSectors: 	    DW 2880
bpbMedia: 	          DB 0xF0
bpbSectorsPerFAT: 	  DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 	  DD 0
bpbTotalSectorsBig:   DD 0
bsDriveNumber: 	      DB 0
bsUnused: 	          DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	      DD 0xa0a1a2a3
bsVolumeLabel: 	      DB "MOS FLOPPY "
bsFileSystem: 	      DB "FAT12   "

msg	db	"Welcome to My Operating System!", 0ah, 0dh, 0h		; the string to print
msg2 db "Hello World!",	0ah, 0dh, 0h
read_fat_msg db "Reading FAT", 	0ah, 0dh, 0h

;***************************************
;	Prints a string
;	DS=>SI: 0 terminated string
;***************************************

Print:
			lodsb					; load next byte from string from SI to AL
			or			al, al		; Does AL=0?
			jz			PrintDone	; Yep, null terminator found-bail out
			mov			ah,	0eh	; Nope-Print the character
      mov     cx, 0abcdh
			int			10h
			jmp			Print		; Repeat until null terminator found
PrintDone:
			ret					; we are done, so return

;*************************************************;
;	Bootloader Entry Point
;*************************************************;

loader:
	xor	ax, ax		; Setup segments to insure they are 0. Remember that
	mov	ds, ax		; we have ORG 0x7c00. This means all addresses are based
	mov	es, ax		; from 0x7c00:0. Because the data segments are within the same
                ; ; code segment, null em.

	mov		ah, 0					; reset floppy disk function
	mov		dl, 0					; drive 0 is floppy drive
	int		0x13					; call BIOS
	jc		loader					; If Carry Flag (CF) is set, there was an error. Try resetting again

	mov		ax, 0x1000				; we are going to read sector to into address 0x1000:0
	mov		es, ax
	xor		bx, bx

  mov		ah, 0x02			; read floppy sector function
	mov		al, 1					; read 1 sector
	mov		ch, 0					; we are reading the second sector past us, so its still on track 0
	mov		cl, 2					; sector to read (The second sector)
	mov		dh, 0					; head number
	mov		dl, 0					; drive number. Remember Drive 0 is floppy drive.
	int		0x13					; call BIOS - Read the sector

	mov si, read_fat_msg
	call Print

  jmp		0x1000:0x0				; jump to execute the sector!

	cli							; Clear all Interrupts
	hlt							; halt the system

	times 510 - ($-$$) db 0					; We have to be 512 bytes. Clear the rest of the bytes with 0
                                  ; $ - $$ means calculating the space used between current address and
	                                ; beginning address of a section.
                                  ; ; We subtract 510 because 2 bytes is used at the end

dw 0xAA55							; Boot Signiture
