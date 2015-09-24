bits	16

org 0x500

; Remember the memory map-- 0x500 through 0x7bff is unused above the BIOS data area.
; We are loaded at 0x500 (0x50:0)

jmp	main				; go to start

;*******************************************************
;	Preprocessor directives
;*******************************************************

%include "stdio.inc"			; basic i/o routines
%include "gdt.inc"			; Gdt routines
%include "a20.inc"
%include "disk16.inc"

;*******************************************************
;	Data Section
;*******************************************************

LoadingMsg db "Preparing to load operating system...", 0ah, 0dh, 0h

;*******************************************************
;	STAGE 2 ENTRY POINT
;
;		-Store BIOS information
;		-Load Kernel
;		-Install GDT; go into protected mode (pmode)
;		-Jump to Stage 3
;*******************************************************

main:
	;-------------------------------;
	;   Enable A20			;
	;-------------------------------;
	call	EnableA20_KKbrd_Out

	; move kernel to address 0x10FF0
	mov	ax, 0x10FF
	mov	es, ax
	xor	bx, bx

  mov	num_of_sectors, 1					; read 1 sector
	mov	track_num, 0					; we are reading the second sector past us, so its still on track 0
	mov	sector_num, 3					; sector to read (The second sector)
	mov	head_num, 0					; head number
	mov	drive_num, 0					; drive number. Remember Drive 0 is floppy drive.
  mov	ah, 0x02			; read floppy sector function
	int	0x13					; call BIOS - Read the sector

	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;
	cli				; clear interrupts
  xor	ax, ax			; null segments
	mov	ds, ax
	mov	es, ax
  mov bx, ax
	mov cx, ax
	mov	ax, 0x9000		; stack begins at 0x9000-0xffff
	mov	ss, ax

	mov	sp, 0xFFFF
	sti				; enable interrupts

	;-------------------------------;
	;   Print loading message	;
	;-------------------------------;
  mov	si, LoadingMsg
	call	Puts16

	;-------------------------------;
	;   Install our GDT		;
	;-------------------------------;

	call	InstallGDT		; install our GDT

	;-------------------------------;
	;   Go into pmode		;
	;-------------------------------;

	cli				; clear interrupts
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	; far jump to fix CS. Remember that the code selector is 0x8! My note: In
  ; protected mode, we refer to a code segment based on the GDT defined earlier
  ; (which is 0x8, 8 bytes away from the start of GDT), not absolute address
  ; anymore and offset from the based address of the code selector. CPU will
  ; know how to resolve the address.
	;
	; In this case, we define that our code segment (and selector) starts from
	; 0x10000 (with a limit of 0xFFFF, so code segment only lasts upto 0x1FFFF)
	; and we load the kernel at 0x10FF0, so the offset is 0xFF0.
	jmp	08h:0xFF0

	; Note: Do NOT re-enable interrupts! Doing so will triple fault!
	; We will fix this in Stage 3.
