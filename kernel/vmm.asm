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
  xchg bx, bx
  mov eax, pt
  mov ebx, PTE_PRESENT | PTE_WRITEABLE | PTE_USER
  call pt_entry_set_attr

  ret

; eax: address to an entry
; ebx: attribute bits to set
pt_entry_set_attr:
  mov ecx, 0
  and [eax], ecx
  or [eax], ebx
  ret

pt_entry_unset_attr:
  
  ret

pt_entry_set_frame:
  ret

pt_entry_is_present:
  ret

pt_entry_is_writable:
  ret

pt_entry_get_pfn:
  ret
