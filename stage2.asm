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

  ; Print green background

  mov ah, 0bh
  mov bh, 00h
  mov bl, 0ffh
  int 0x10

	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;
	cli				; clear interrupts
  xor	ax, ax			; null segments
	mov	ds, ax
	mov	es, ax
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

	jmp	08h:Stage3		; far jump to fix CS. Remember that the code selector is 0x8!

	; Note: Do NOT re-enable interrupts! Doing so will triple fault!
	; We will fix this in Stage 3.

;******************************************************
;	ENTRY POINT FOR STAGE 3
;******************************************************

bits 32					; Welcome to the 32 bit world!

Stage3:

	;-------------------------------;
	;   Set registers		;
	;-------------------------------;

	mov		ax, 0x10		; set data segments to data selector (0x10)
	mov		ds, ax
	mov		ss, ax
	mov		es, ax
	mov		esp, 90000h		; stack begins from 90000h

;*******************************************************
;	Stop execution
;*******************************************************

STOP:
	cli
	hlt
