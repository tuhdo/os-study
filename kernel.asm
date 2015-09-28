org 0x10FF0

bits 32

  cmp bl, 1
  je UserMode
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

UserMode:
  call 0x28:0xFF0
  retf

Stage3:
  ;-------------------------------;
  ;   Set registers		;
  ;-------------------------------;
  mov		ax, 0x10		; set data segments to data selector (0x10)
  mov		ds, ax
  mov		ss, ax
  mov		es, ax
  mov		esp, 90000h		; stack begins from 90000h
  mov   edi, 0xFFFFFFFF         ; test 32 bit

  call ClrScr32
  mov eax, WelcomeMsg
  call Puts32

  ; AMAZING
  ; Link to read for understanding: http://www.jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
  cli
  mov ax,0x23
  mov ds,ax
  mov es,ax
  mov fs,ax
  mov gs,ax ;we don't need to worry about SS. it's handled by iret

  mov eax, esp
  push 0x23 ;user data segment with bottom 2 bits set for ring 3
  push eax ;push our current stack just for the heck of it
  pushf
  push 0x1b; ;user data segment with bottom 2 bits set for ring 3
  push 0xFF0 ;may need to remove the _ for this to work right 
  iret

  ;*******************************************************
  ;	Stop execution
  ;*******************************************************

STOP:
  cli
  hlt
