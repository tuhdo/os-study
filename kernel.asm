org 0x10000

bits 32

jmp Stage3
; Print green background

; mov ah, 0bh
; mov bh, 00h
; mov bl, 0ffh
; int 0x10

;******************************************************
;	ENTRY POINT FOR STAGE 3
;******************************************************
	; Welcome to the 32 bit world!

%include "stdio32.inc"

WelcomeMsg db "Welcome to Tu's Operating System", 0ah, 0h

Stage3:
  hlt
	;-------------------------------;
	;   Set registers		;
	;-------------------------------;
	mov		ax, 0x10		; set data segments to data selector (0x10)
	mov		ds, ax
	mov		ss, ax
	mov		es, ax
	mov		esp, 90000h		; stack begins from 90000h
	mov   edi, 0xFFFFFFFF 				; test 32 bit

	call ClrScr32
	mov eax, WelcomeMsg
	call Puts32
	cli
  
  ;*******************************************************
  ;	Stop execution
  ;*******************************************************

STOP:
	cli
	hlt
