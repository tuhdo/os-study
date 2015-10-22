BiosGetMemorySize:
  int 0x12
  jc .error
  test ax, ax
  je .error
  cmp ah, 0x80
  je .error
  cmp ah, 0x86
  je .error
  ret
.error:
  mov ax, -1
  ret

BiosGetExtendedMemorySize:
  mov	ax, 0x88
  int	0x15
  jc	.error
  test	ax, ax		; if size=0
  je	.error
  cmp	ah, 0x86	;unsupported function
  je	.error
  cmp	ah, 0x80	;invalid command
  je	.error
  ret
.error:
  mov	ax, -1
  ret

;---------------------------------------------
;	Get memory size for >64M configuations
;	ret\ ax=KB between 1MB and 16MB
;	ret\ bx=number of 64K blocks above 16MB
;	ret\ bx=0 and ax= -1 on error
;---------------------------------------------

BiosGetMemorySize64MB:
  push	ecx
  push	edx
  xor	ecx, ecx		;clear all registers. This is needed for testing later
  xor	edx, edx
  mov	ax, 0xe801
  int	0x15
  jc	.error
  cmp	ah, 0x86		;unsupported function
  je	.error
  cmp	ah, 0x80		;invalid command
  je	.error
  jcxz	.use_ax			; bios may have stored it in ax,bx or cx,dx. test if cx is 0
  mov	ax, cx			  ; its not, so it should contain mem size; store it.
  mov	bx, dx        ; jcxz means "jump cx zero"

.use_ax:
  pop	edx			;mem size is in ax and bx already, return it
  pop	ecx
  ret

.error:
  mov	ax, -1
  mov	bx, 0
  pop	edx
  pop	ecx
  ret

;;;;;;;;;;;;;;;;;;;;;;
; GETTING MEMORY MAP ;
;;;;;;;;;;;;;;;;;;;;;;
; Types of Address Ranges

; The types of address ranges defined for this function is shown below:

; 1: Available Memory
; 2: Reserved, do not use. (e.g. system ROM, memory-mapped device)
; 3: ACPI Reclaim Memory (usable by OS after reading ACPI tables)
; 4: ACPI NVS Memory (OS is required to save this memory between NVS sessions)
; All other values should be treated as undefined.

struc	MemoryMapEntry
	.baseAddress	resq	1	; base address of address range
	.length		resq	1	; length of address range in bytes
	.type		resd	1	; type of address range
	.acpi_null	resd	1	; reserved
endstruc


;---------------------------------------------
;	Get memory map from bios
;	/in es:di->destination buffer for entries
;	/ret bp=entry count
;---------------------------------------------
BiosGetMemoryMap:
	pushad
	xor	ebx, ebx
	xor	bp, bp			; number of entries stored here
	mov	edx, 'PAMS'		; 'SMAP'
	mov	eax, 0xe820
	mov	ecx, 24			; memory map entry struct is 24 bytes
	int	0x15			; get first entry
	jc	.error
	cmp	eax, 'PAMS'		; bios returns SMAP in eax
	jne	.error
	test	ebx, ebx		; if ebx=0 then list is one entry long; bail out
	je	.error
	jmp	.start
.next_entry:
	mov	edx, 'PAMS'		; some bios's trash this register
	mov	ecx, 24			; entry is 24 bytes
	mov	eax, 0xe820
	int	0x15			; get next entry
.start:
	jcxz	.skip_entry		; if actual returned bytes is 0, skip entry
.notext:
	mov	ecx, [es:di + MemoryMapEntry.length]	; get length (low dword)
	test	ecx, ecx		; if length is 0 skip it
	jne	short .good_entry
	mov	ecx, [es:di + MemoryMapEntry.length + 4]; get length (upper dword)
	jecxz	.skip_entry		; if length is 0 skip it
.good_entry:
	inc	bp			; increment entry count
	add	di, 24			; point di to next entry in buffer
.skip_entry:
	cmp	ebx, 0			; if ebx return is 0, list is done
	jne	.next_entry		; get next entry
	jmp	.done
.error:
	stc                           ; set CF in EFLAGS register
.done:
	popad
	ret

struc multiboot_info
	.flags		resd	1	; required
	.memoryLo	resd	1	; memory size. Present if flags[0] is set
	.memoryHi	resd	1
	.bootDevice	resd	1	; boot device. Present if flags[1] is set
	.cmdLine	resd	1	; kernel command line. Present if flags[2] is set
	.mods_count	resd	1	; number of modules loaded along with kernel. present if flags[3] is set
	.mods_addr	resd	1
	.syms0		resd	1	; symbol table info. present if flags[4] or flags[5] is set
	.syms1		resd	1
	.syms2		resd	1
	.mmap_length	resd	1	; memory map. Present if flags[6] is set
	.mmap_addr	resd	1
	.drives_length	resd	1	; phys address of first drive structure. present if flags[7] is set
	.drives_addr	resd	1
	.config_table	resd	1	; ROM configuation table. present if flags[8] is set
	.bootloader_name resd	1	; Bootloader name. present if flags[9] is set
	.apm_table	resd	1	; advanced power management (apm) table. present if flags[10] is set
	.vbe_control_info resd	1	; video bios extension (vbe). present if flags[11] is set
	.vbe_mode_info	resd	1
	.vbe_mode	resw	1
	.vbe_interface_seg resw	1
	.vbe_interface_off resw	1
	.vbe_interface_len resw	1
endstruc
