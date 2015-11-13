org 0x50000

bits 32

cmp edi, 0
jmp vmm_test

cmp edi, 1
jmp pt_entry_set_attr

%define PTE_PRESENT       0x1
%define PTE_WRITEABLE     0x2
%define PTE_USER          0x4
%define PTE_WRITETHROUGH  0x8
%define PTE_NOT_CACHABLE  0x10
%define PTE_ACCESS        0x20
%define PTE_DIRTY         0x40
%define PTE_PAT           0x80
%define PTE_CPU_GLOBAL    0x100
%define PTE_LV4_GLObAL    0x200
%define PTE_ATTRS          0x00000FFF
%define PTE_FRAME         0x7FFFF000

%define PDE_PRESENT       0x1
%define PDE_WRITEABLE     0x2
%define PDE_USER          0x4
%define PDE_PWT           0x8
%define PDE_PCD           0x10
%define PDE_ACCESS        0x20
%define PDE_DIRTY         0x40
%define PDE_4MB           0x80
%define PDE_CPU_GLOBAL    0x100
%define PDE_LV4_GLObAL    0x200
%define PDE_FRAME         0x7FFFF000

pt times 1024 dd 0
pdt times 1024 dd 0

vmm_test:
  mov eax, pt
  mov ebx, PTE_PRESENT | PTE_WRITEABLE | PTE_USER
  call pt_entry_set_attr

  mov ebx, PTE_USER
  call pt_entry_unset_attr

  mov ebx, 0x10000
  call pt_entry_set_frame

  ; get page frame address
  mov eax, pt
  call pt_entry_get_pfn

  ; check if present
  xchg bx, bx
  mov eax, pt
  call pt_entry_is_present

  ;check if writable
  mov eax, pt
  call pt_entry_is_writable

  retf

; eax: address to an entry
; ebx: attribute bits to set
pt_entry_set_attr:
  mov ecx, PTE_ATTRS
  and ecx, [eax]
  or ecx, ebx
  mov [eax], ecx
  ret

; eax: address to an entry
; ebx: attribute to unset
pt_entry_unset_attr:
  xor [eax], ebx
  ret

; eax: address to an entry
; ebx: frame address to set
pt_entry_set_frame:
  xchg bx, bx
  mov ecx, PTE_ATTRS
  and [eax], ecx
  mov cl, 3
  shr ebx, cl
  shl ebx, cl
  or [eax], ebx

  ret

; eax: address to entry
; return present status, in eax
pt_entry_is_present:
  mov ebx, PTE_PRESENT
  and ebx, [eax]
  mov eax, ebx

  ret

; eax: address to entry
; return writable status, in eax
pt_entry_is_writable:
  mov ebx, PTE_WRITEABLE
  and ebx, [eax]
  mov eax, ebx
  ret

; eax: address to entry
; return frame address this page is managing
pt_entry_get_pfn:
  mov ebx, PTE_FRAME
  and ebx, [eax]
  mov eax, ebx

  ret
