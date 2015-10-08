org 0x20000

bits 32

cmp ecx, 0
je int_handler0

cmp ecx, 1
je int_handler1

%include "stdio32.inc"

DivByZeroMsg  db "Divide by Zero", 0ah, 0h
InterruptMsg  db "Calling from interrupt", 0ah, 0h

int_handler0:
  mov bl, 0
  mov bh, 0
  call MovCur

  mov eax, DivByZeroMsg
  call Puts32

  mov al, 1
  mov dl, 1

  jmp ireturn

int_handler1:
  mov bl, 5
  mov bh, 5
  ; mov eax, MovCur
  ; and eax, 0xFFFF
  call MovCur

  ; mov ebx, Puts32
  ; and ebx, 0xFFFF
  mov eax, InterruptMsg
  ; call 0x38:0x1008a
  call MovCur

  pusha
  ; mov		ax, 0x10		; set data segments to data selector (0x10)
  ; mov		ds, ax

  ; mov bl, 5
  ; mov bh, 5
  ; call MovCur

  ; ; restore back the stack for userspace afterward
  ; mov ax, 0x23
  ; mov ds, ax
  popa

  ; only use lower 16 bits, since we use segment in protected mode.
  ; This is necessary because int instruction stores absolute address, while iret instruction use
  ; segmented address.

ireturn:
  mov eax, [esp]
  and eax, 0xFFFF
  mov [esp], eax

  iret
