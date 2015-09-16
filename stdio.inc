  ;*************************************************
  ;	stdio.inc
  ;		-Input/Output routines
  ;
  ;	OS Development Series
  ;*************************************************

%ifndef __STDIO_INC_67343546FDCC56AAB872_INCLUDED__
%define __STDIO_INC_67343546FDCC56AAB872_INCLUDED__

  ;************************************************;
  ;	Puts16 ()
  ;		-Prints a null terminated string
  ;	DS=>SI: 0 terminated string
  ;************************************************;

bits	16

Print:
			lodsb					; load next byte from string from SI to AL
			or			al, al		; Does AL=0?
			jz			PrintDone	; Yep, null terminator found-bail out
			mov			ah,	0eh	; Nope-Print the character
			int			10h
			jmp			Print		; Repeat until null terminator found
PrintDone:
			ret					; we are done, so return

Puts16:
  pusha				; save registers
  .Loop1:
  lodsb				; load next byte from string from SI to AL
  or	al, al			; Does AL=0?
  jz	Puts16Done		; Yep, null terminator found-bail out
  mov	ah, 0eh			; Nope-Print the character
  mov cx, 0abcdh
  int	10h			; invoke BIOS
  jmp	.Loop1			; Repeat until null terminator found
Puts16Done:
  popa				; restore registers
  ret				; we are done, so return

%endif ;__STDIO_INC_67343546FDCC56AAB872_INCLUDED__
