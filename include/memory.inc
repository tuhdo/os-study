bits 16

%include "mem_info.inc"

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
;---------------------------------------------
;	Get memory map from bios
;	/in es:di->destination buffer for entries
;	/ret eax=entry count
;---------------------------------------------
BiosGetMemoryMap:
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
  mov eax, ebp
	ret
