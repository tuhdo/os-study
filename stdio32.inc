_CurX db 5
_CurY db 5

%define VIDMEM        0xB8000
%define COLS          80
%define LINES         25
%define CHAR_ATTRIB   15

  ;**************************************************;
  ;	Putch32 ()
  ;		- Prints a character to screen
  ;	BL => Character to print
  ;**************************************************;

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
	mov word [edi], dx		; write to video display

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

Puts32:
  pusha
  ; Print a character at a time
.loop_Puts32:
  cmp byte [eax], 0
  je .done_Puts32
  mov bl, [eax]
  call Putch32
  inc eax
  jmp .loop_Puts32
.done_Puts32:
  popa
  ret

;**************************************************;
;	MoveCur ()
;		- Update hardware cursor
;	parm/ bh = Y pos
;	parm/ bl = x pos
;**************************************************;
MovCur:
 	pusha				; save registers (aren't you getting tired of this comment?)
 	;-------------------------------;
	;   Get current position        ;
	;-------------------------------;
 	; Here, _CurX and _CurY are relitave to the current position on screen, not in memory.
	; That is, we don't need to worry about the byte alignment we do when displaying characters,
	; so just follow the forumla: location = _CurX + _CurY * COLS
	xor	eax, eax
	mov	ecx, COLS
  mov	byte [_CurX], bl
	mov	byte [_CurY], bh
	mov	al, bh			; get y pos
	mul	ecx			; multiply y*COLS
	add	al, bl			; Now add x
	mov	ebx, eax

	;--------------------------------------;
	;   Set low byte index to VGA register ;
	;--------------------------------------;
	mov	al, 0x0f		; Cursor location low byte index
	mov	dx, 0x03D4		; Write it to the CRT index register
	out	dx, al

	mov	al, bl			; The current location is in EBX. BL contains the low byte, BH high byte
	mov	dx, 0x03D5		; Write it to the data register
	out	dx, al			; low byte

	;---------------------------------------;
	;   Set high byte index to VGA register ;
	;---------------------------------------;
	xor	eax, eax

	mov	al, 0x0e		; Cursor location high byte index
	mov	dx, 0x03D4		; Write to the CRT index register
	out	dx, al

	mov	al, bh			; the current location is in EBX. BL contains low byte, BH high byte
	mov	dx, 0x03D5		; Write it to the data register
	out	dx, al			; high byte

	popa
	ret

;**************************************************;
;	ClrScr32 ()
;		- Clears screen
;**************************************************;
ClrScr32:
	pusha
	cld
	mov	edi, VIDMEM
	mov	cx, 2000
	mov	ah, CHAR_ATTRIB
	mov	al, ' '
  ; rep to repeat an instruction CX times. stosw to store al to the address in
  ; edi, then increment by one. the amount depends on which stos[b/w/d]
  ; instruction is used. in this case, we use stosw because video memory include
  ; 2 bytes: the first is character and the 2nd is the attrubte of that
  ; character. We only want to modify the first byte every 2 bytes.
	rep	stosw
	popa
	ret
