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

_CurX db 20
_CurY db 15

%define VIDMEM        0xB8000
%define COLS          80
%define LINES         25
%define CHAR_ATTRIB   14

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

Putch32:
  pusha				; save registers
  mov	edi, VIDMEM		; get pointer to video memory

  ;-------------------------------;
  ;   Get current position	;
  ;-------------------------------;
  xor	eax, eax		; clear eax

  ;--------------------------------
  ; Remember: currentPos = x + y * COLS! x and y are in _CurX and _CurY.
  ; Because there are two bytes per character, COLS=number of characters in a line.
  ; We have to multiply this by 2 to get number of bytes per line. This is the screen width,
  ; so multiply screen with * _CurY to get current line
  ;--------------------------------
  mov	ecx, COLS*2		; Mode 7 has 2 bytes per char, so its COLS*2 bytes per line
  mov	al, byte [_CurY]	; get y pos
  mul	ecx			; multiply y*COLS
  push	eax			; save eax--the multiplication

  ;--------------------------------
  ; Now y * screen width is in eax. Now, just add _CurX. But, again remember that _CurX is relative
  ; to the current character count, not byte count. Because there are two bytes per character, we
  ; have to multiply _CurX by 2 first, then add it to our screen width * y.
  ;--------------------------------
  mov	al, byte [_CurX]	; multiply _CurX by 2 because it is 2 bytes per char
  mov	cl, 2
  mul	cl
  pop	ecx			; pop y*COLS result
  add	eax, ecx

  ;-------------------------------
  ; Now eax contains the offset address to draw the character at, so just add it to the base address
  ; of video memory (Stored in edi)
  ;-------------------------------
  xor	ecx, ecx
  add	edi, eax		; add it to the base address


  ;-------------------------------;
  ;   Watch for new line          ;
  ;-------------------------------;
  cmp	bl, 0x0A		; is it a newline character?
  je	.Row			; yep--go to next row

  ;-------------------------------;
  ;   Print a character           ;
  ;-------------------------------;
  mov	dl, bl			; Get character
  mov	dh, CHAR_ATTRIB		; the character attribute
  push eax
  mov ax, 0xB800
  mov es, ax
  pop ecx
  mov	[es:ecx], dx		; write to video display

  ;-------------------------------;
  ;   Update next position        ;
  ;-------------------------------;
  inc	byte [_CurX]		; go to next character
  cmp byte	[_CurX], COLS		; are we at the end of the line?
  je	.Row			; yep-go to next row
  jmp	.done			; nope, bail out

	;-------------------------------;
	;   Go to next row              ;
	;-------------------------------;
.Row:
	mov	byte [_CurX], 0		; go back to col 0
	inc	byte [_CurY]		; go to next row

	;-------------------------------;
	;   Restore registers & return  ;
	;-------------------------------;

.done:
	popa				; restore registers and return
	ret

loader:
	xor	ax, ax		; Setup segments to insure they are 0. Remember that
	mov	ds, ax		; we have ORG 0x7c00. This means all addresses are based
	mov	es, ax		; from 0x7c00:0. Because the data segments are within the same
                ; ; code segment, null em.

	mov		ah, 0					; reset floppy disk function
	mov		dl, 0					; drive 0 is floppy drive
	int		0x13					; call BIOS
	jc		loader					; If Carry Flag (CF) is set, there was an error. Try resetting again

	mov		ax, 0x50				; we are going to read sector to into address 0x50:0
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

	; mov ax, 0xB800
	; mov es, ax
	; mov byte [es:0], 'z'

	; mov bl, 'Z'
  ; call Putch32

  jmp		0x50:0x0				; jump to execute the sector!

	; cli							; Clear all Interrupts
	; hlt							; halt the system

	times 510 - ($-$$) db 0					; We have to be 512 bytes. Clear the rest of the bytes with 0
                                  ; $ - $$ means calculating the space used between current address and
	                                ; beginning address of a section.
                                  ; ; We subtract 510 because 2 bytes is used at the end

dw 0xAA55							; Boot Signiture
