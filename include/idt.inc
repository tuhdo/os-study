idt: times 256 dq 0.0 ; 256 entries

DivByZeroMsg  db "Divide by Zero", 0ah, 0h
InterruptMsg  db "Calling from interrupt", 0ah, 0h
KeyboardMsg   db "Got a scancode from keyboard", 0ah, 0h
PicIntrMsg    db "Receive timer interrupt from PIC!", 0ah, 0h
UnhandledMsg  db "Unhandled interrupt: ",  0h
IntNum        db "0", 0h

isrs:
  dd _isr0
  dd _isr1
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _irq_unhandled
  dd _isr32
  dd _isr33
  dd _isr32
  dd _isr33
  dd _isr32
  dd _isr33
  dd _isr32
  dd _isr33
  dd _isr32
  dd _isr33
  dd _isr32

_irq_unhandled:
  mov bl, 10
  mov bh, 5
  call MovCur

  mov eax, UnhandledMsg
  call Puts32

  add cl, 0x30
  mov [IntNum], cl
  mov eax, IntNum
  call Puts32

  ret

_isr0:
  mov bl, 0
  mov bh, 0
  call MovCur

  mov eax, DivByZeroMsg
  call Puts32

  mov al, 1
  mov dl, 1
  ret

_isr1:
  mov bl, 2
  mov bh, 2
  ; mov eax, MovCur
  ; and eax, 0xFFFF
  call MovCur

  mov eax, KeyboardMsg
  ; and eax, 0xFFFF  ; retrieve offset only when base address is something different than 0
  ; call 0x38:0x1008a
  call Puts32
  ret

_isr32:
  mov bl, 5
  mov bh, 15
  ; mov eax, MovCur
  ; and eax, 0xFFFF
  call MovCur

  mov eax, PicIntrMsg
  ; and eax, 0xFFFF  ; retrieve offset only when base address is something different than 0
  ; call 0x38:0x1008a
  call Puts32

  mov al, 0x20
  out 0x20, al
  ret

_isr33:
  mov bl, 2
  mov bh, 2
  ; mov eax, MovCur
  ; and eax, 0xFFFF
  call MovCur

  mov eax, KeyboardMsg
  ; and eax, 0xFFFF  ; retrieve offset only when base address is something different than 0
  ; call 0x38:0x1008a
  call Puts32

  ; send EOI to primary PIC

	mov	al, 0x20	; set bit 4 of OCW 2
	out	0x20, al	; write to primary PIC command register
  ret

%macro isr_no_error_code 1
[GLOBAL isr%1]
isr%1:
  cli
  push byte 0
  push byte %1
  jmp irq_common
  ; xchg bx, bx
%endmacro

%macro isr_error_code 1
[GLOBAL isr%1]
isr%1:
  cli
  push byte %1
  jmp irq_common
%endmacro

;; 1. Store all current registers.
;; 2. Push current ds onto stack for restoring later.
;; 3. Load ds, es, fs, gs with kernel data descriptor.
;; 5. Run interrupt handler.
;; 6. Restore registers back to its original states.
irq_common:
  xor ecx, ecx
  mov cl, [esp]            ; get IRQ number
  pushad

  mov ax, ds               ; Lower 16-bits of eax = ds.
  push eax
  mov ax, 0x10  ; load the kernel data segment descriptor
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ; calculate index into isrs table
  xor eax, eax
  mov al, cl            ; load IRQ number
  mov bl, 4
  mul bl
  mov ebx, eax
  mov eax, isrs
  add eax, ebx
  mov eax, [eax]
  and eax, 0xFFFF

  call eax

  pop eax        ; reload the original registers
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  popad

  ; only use lower 16 bits, since we use segment in protected mode.
  ; This is necessary because int instruction stores absolute address, while iret instruction use
  ; segmented address.
  mov ecx, [esp + 8]
  and ecx, 0xFFFF
  mov [esp + 8], ecx

  ; remove error code and IRQ number from the stack to return
  pop ecx
  pop ecx

  sti

  iret

;; with error code
isr_error_code    8
isr_error_code    10
isr_error_code    11
isr_error_code    12
isr_error_code    13
isr_error_code    14
isr_error_code    17
isr_error_code    30

;; without error code
isr_no_error_code 0
isr_no_error_code 1
isr_no_error_code 2
isr_no_error_code 3
isr_no_error_code 4
isr_no_error_code 5
isr_no_error_code 6
isr_no_error_code 7
isr_no_error_code 9
isr_no_error_code 15
isr_no_error_code 16
isr_no_error_code 18
isr_no_error_code 19
isr_no_error_code 20
isr_no_error_code 21
isr_no_error_code 22
isr_no_error_code 23
isr_no_error_code 24
isr_no_error_code 25
isr_no_error_code 26
isr_no_error_code 27
isr_no_error_code 28
isr_no_error_code 29
isr_no_error_code 31
isr_no_error_code 32
isr_no_error_code 33
isr_no_error_code 34
isr_no_error_code 35
isr_no_error_code 36
isr_no_error_code 37
; isr_no_error_code 38
; isr_no_error_code 39
; isr_no_error_code 40
; isr_no_error_code 41
; isr_no_error_code 42

%macro install_isr 1
  xor eax, eax
  mov eax, %1
  mov bx, 8
  mul bx
  add eax, idt

  mov ebx, isr%1
  and ebx, 0xFFFF
  ; mov ecx, _isr%1
  ; shr ecx, 16
  ; and ecx, 0xFFFF
  mov word [eax], bx
  mov word [eax + 2], 0x0030
  mov byte [eax + 4], 0x0
  mov byte [eax + 5], 10001110b
  mov word [eax + 6], 0
%endmacro

InstallIDT:
  cli				; clear interrupts
	pusha				; save registers
  install_isr 0
  install_isr 1
  install_isr 2
  install_isr 3
  install_isr 4
  install_isr 5
  install_isr 6
  install_isr 7
  install_isr 8
  install_isr 9
  install_isr 10
  install_isr 11
  install_isr 12
  install_isr 13
  install_isr 14
  install_isr 15
  install_isr 16
  install_isr 17
  install_isr 18
  install_isr 19
  install_isr 20
  install_isr 21
  install_isr 22
  install_isr 23
  install_isr 24
  install_isr 25
  install_isr 26
  install_isr 27
  install_isr 28
  install_isr 29
  install_isr 30
  install_isr 31
  install_isr 32
  install_isr 33
  install_isr 34
  install_isr 35
  install_isr 36
  install_isr 37
  ; install_isr 38
  ; install_isr 39
  ; install_isr 40
  ; install_isr 41
  ; install_isr 42
  lidt	[idt_ptr]			; load GDT into GDTR
  sti
  popa
  ret

idt_end:

idt_ptr:
	.limit dw (256 * 8) - 1	; bits 0...15 is size of idt
	.base	dd idt		; base of idt

  ; load register with idt_ptr
