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
%include "memory.inc"

;*******************************************************
;	Data Section
;*******************************************************

LoadingMsg db "Preparing to load operating system...", 0ah, 0dh, 0h

boot_info:
istruc multiboot_info
	at multiboot_info.flags,							dd 0
	at multiboot_info.memoryLo,						dd 0
	at multiboot_info.memoryHi,						dd 0
	at multiboot_info.bootDevice,					dd 0
	at multiboot_info.cmdLine,						dd 0
	at multiboot_info.mods_count,					dd 0
	at multiboot_info.mods_addr,					dd 0
	at multiboot_info.syms0,							dd 0
	at multiboot_info.syms1,							dd 0
	at multiboot_info.syms2,							dd 0
	at multiboot_info.mmap_length,				dd 0
	at multiboot_info.mmap_addr,					dd 0
	at multiboot_info.drives_length,			dd 0
	at multiboot_info.drives_addr,				dd 0
	at multiboot_info.config_table,				dd 0
	at multiboot_info.bootloader_name,		dd 0
	at multiboot_info.apm_table,					dd 0
	at multiboot_info.vbe_control_info,		dd 0
	at multiboot_info.vbe_mode_info,			dw 0
	at multiboot_info.vbe_interface_seg,	dw 0
	at multiboot_info.vbe_interface_off,	dw 0
	at multiboot_info.vbe_interface_len,	dw 0
iend

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

  mov	num_of_sectors, 15					; read 15 sector
	mov	track_num, 0					; we are reading the 4th sector past us, so its still on track 0
	mov	sector_num, 4					; sector to read (The 4th sector)
	mov	head_num, 0					; head number
	mov	drive_num, 0					; drive number. Remember Drive 0 is floppy drive.
  mov	ah, 0x02			; read floppy sector function
	int	0x13					; call BIOS - Read the sector

	; interrupt
	mov	ax, 0x2000
	mov	es, ax
	xor	bx, bx

	mov	num_of_sectors, 12					; read 15 sector
	mov	track_num, 0					; we are reading the 19th sector past us, so its still on track 1
	mov	sector_num, 1				; sector to read (The 19th sector)
	mov	head_num, 1					; head number
	mov	drive_num, 0					; drive number. Remember Drive 0 is floppy drive.
  mov	ah, 0x02			; read floppy sector function
	int	0x13					; call BIOS - Read the sector

	; userspace
	mov	ax, 0x40FF
	mov	es, ax
	xor	bx, bx

	mov	num_of_sectors, 6					; read 1 sector
	mov	track_num, 0					; we are reading the 9th sector past us, so its still on track 0
	mov	sector_num, 13					; sector to read (The 9th sector)
	mov	head_num, 1					; head number
	mov	drive_num, 0					; drive number. Remember Drive 0 is floppy drive.
  mov	ah, 0x02			; read floppy sector function
	int	0x13					; call BIOS - Read the sector

	; virtual memory manager code
	mov	ax, 0x5000
	mov	es, ax
	xor	bx, bx

	mov	num_of_sectors, 18					; read 1 sector
	mov	track_num, 1					; we are reading the 9th sector past us, so its still on track 0
	mov	sector_num, 1					; sector to read (The 9th sector)
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
	; sti				; enable interrupts

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
	; Prepare memroy information
	;-------------------------------;
	xor		eax, eax
	xor		ebx, ebx
  call BiosGetMemorySize64MB
	mov	[boot_info+multiboot_info.memoryHi], bx
	mov	[boot_info+multiboot_info.memoryLo], ax
	mov word [boot_info+multiboot_info.bootDevice], 0x0

  mov edi, 0x2000
	call BiosGetMemoryMap
  mov word [boot_info+multiboot_info.mmap_length], ax
	mov word [boot_info+multiboot_info.mmap_addr], 0x2000

	;-------------------------------;
	;   Go into pmode		;
	;-------------------------------;
	cli
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax
	; The Multiboot specification states that, when we invoke a 32 bit operating system (That is, execute our kernel), the machine registers must be set to a specific state. More specifically: When we execute our kernel, set up the registers to the following values:
	; - EAX - Magic Number. Must be 0x2BADB002. This will indicate to the kernel
  ; that our boot loader is multiboot standard
	;
  ; - EBX - Containes the physical address of the Multiboot information structure
	;
  ; - CS - Must be a 32-bit read/execute code segment with an offset of `0' and
  ; a limit of `0xFFFFFFFF'. The exact value is undefined.
	;
  ; - DS,ES,FS,GS,SS - Must be a 32-bit read/write data segment with an offset
  ; of `0' and a limit of `0xFFFFFFFF'. The exact values are all undefined.
	;
  ; - A20 gate must be enabled
	;
  ; - CR0 - Bit 31 (PG) bit must be cleared (paging disabled) and Bit 0 (PE) bit
  ; must be set (Protected Mode enabled). Other bits undefined
	mov eax, 0x2badb002
	mov ebx, boot_info

	; far jump to fix CS. Remember that the code selector is 0x8! My note: In
  ; protected mode, we refer to a code segment based on the GDT defined earlier
  ; (which is 0x8, 8 bytes away from the start of GDT), not absolute address
  ; anymore and offset from the based address of the code selector. CPU will
  ; know how to resolve the address.
	;
	; In this case, we define that our code segment (and selector) starts from
	; 0x10000 (with a limit of 0xFFFF, so code segment only lasts upto 0x1FFFF)
	; and we load the kernel at 0x10FF0, so the offset is 0xFF0.
  push dword boot_info
	jmp	08h:0xFF0

	; Note: Do NOT re-enable interrupts! Doing so will triple fault!
	; We will fix this in Stage 3.
