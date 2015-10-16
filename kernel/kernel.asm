org 0x10FF0

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
%include "pic.inc"

WelcomeMsg db "Welcome to Tu's Operating System", 0ah, 0h
InterruptMsg  db "Interrupting", 0ah, 0h
GoodbyeMsg db "See ya later", 0ah, 0h

%define IA32_SYSENTER_CS 0x174
%define IA32_SYSENTER_ESP 0x175
%define IA32_SYSENTER_EIP 0x176

sysenter_setup:
  ; MSR[ECX] <- EDX:EAX
  ; Writes the contents of registers EDX:EAX into the 64-bit model specific
  ; register (MSR) speci- fied in the ECX register. The high-order 32 bits are
  ; copied from EDX and the low-order 32 bits are copied from EAX. Always set
  ; the undefined or reserved bits in an MSR to the values previ- ously read.
	mov	eax, 0x8				; kernel code descriptor
	mov	edx, 0
	mov	ecx, IA32_SYSENTER_CS
	wrmsr

	mov	eax, esp
	mov	edx, 0
	mov	ecx, IA32_SYSENTER_ESP
	wrmsr

	mov	eax, Sysenter_Entry
	mov	edx, 0
	mov	ecx, IA32_SYSENTER_EIP
	wrmsr
  ret

Sysenter_Entry:
  mov		bx, 0x10		; set data segments to data selector (0x10)
  mov		ds, bx
	; sysenter jumps here, is is executing this code at prividege level 0. Simular to Call Gates, normally we will
	; provide a single entry point for all system calls.
  cmp eax, 0
  je clrscr

  cmp eax, 1
  je monitor_out

  cmp eax, 2
  je test_intr_kernel_space

  cmp eax, 3
  je test_intr_pic

  cmp eax, 4
  je STOP
  ; mov eax, GoodbyeMsg
  ; call Puts32
syscall_exit:
  ; restore back the stack for userspace afterward
  mov bx, 0x23
  mov ds, bx
  sysexit

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

  call sysenter_setup

  ;-------------------------------;
	;   Install our IDT		;
	;-------------------------------;

	call	0x30:0		; install our IDT

  call ClrScr32

  mov bl, 20
  mov bh, 5
  call MovCur

  mov eax, WelcomeMsg
  call Puts32

  call MapPIC
  call EnablePIC

  ; AMAZING
  ; Link to read for understanding: http://www.jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
  cli
  mov ax,0x23
  mov ds,ax
  mov es,ax
  mov fs,ax
  mov gs,ax ;we don't need to worry about SS. it's handled by iret

  mov eax, esp
  push 0x23 ; user data segment with bottom 2 bits set for ring 3
  push eax ; push our current stack just for the heck of it
  pushf
  push 0x1b; ;user code segment with bottom 2 bits set for ring 3
  push 0xFF0 ; offset address
  iret

  ;*******************************************************
  ;	Stop execution
  ;*******************************************************

monitor_out:
  ; use kernel data segment, not userspace
  ; we must explicitly set, otherwise when we are trying to retrieve the
  ; data, 0x23 is used with eax for indexing instead.
  ; mov		ax, 0x10		; set data segments to data selector (0x10)
  ; mov		ds, ax

  mov bl, 20
  mov bh, 10
  call MovCur

  mov eax, GoodbyeMsg
  call Puts32

  ; restore back the stack for userspace afterward
  ; mov ax, 0x23
  ; mov ds, ax
  jmp syscall_exit

clrscr:
  call ClrScr32
  jmp syscall_exit

test_intr_kernel_space:
  ; mov ecx, 1
  int 1

  ; ; push syscall_exit
  mov ecx, 0
  mov ax, 3
  mov dl, 0
  div dl
  ; mov	ax, 0x10		; set data segments to data selector (0x10)
  ; mov	ds, ax

  ; mov bl, 5
  ; mov bh, 5
  ; call MovCur

  ; mov eax, InterruptMsg
  ; call Puts32
  jmp syscall_exit

test_intr_pic:
  jmp syscall_exit

STOP:
  cli
  hlt

