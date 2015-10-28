bits 32

%define BLOCK_PER_BYTE 8
%define BLOCK_SIZE 4096

%include "mem_info.inc"

mem_total dd 0
mem_block_total dd 0
mem_block_used dd 0
mem_block_array dd 0

; eax: memory between 1MB and 16MB in KB
; ebx: memory above 16MB in 64KB blocks
; return number of available memory in bytes
get_total_memory:
  ; calculate memory below 1MB
  ; xchg bx, bx
  mov edx, 1024
  mul edx
  push eax

  ; calculate total number of memory above 1MB in KB
  mov eax, 64                   ; it's 64 KB block
  mul ebx                       ; calculate total number in KB
  mov edx, 1024
  mul edx                       ; calculate total number in bytes

  pop ebx
  add eax, ebx
  add eax, 1024*1024            ; add the first 1MB to get the total memory size

  ret
