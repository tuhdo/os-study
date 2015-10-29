bits 32

%define BLOCK_PER_BYTE 8
%define BLOCK_SIZE 4096

%include "mem_info.inc"

TotalMemMsg db "Total number of memory: ", 0h
TotalBlockMsg db "Total number of blocks: ", 0h
Buffer times 100 db "0", 0h
TotalMem db 0
boot_info  dd 0

mem_total dd 0
mem_block_total dd 0
mem_block_used dd 0
mem_block_array dd 0

; eax: boot_info
mem_report:
  mov [boot_info], eax

  mov bl, 0
  mov bh, 1
  call MovCur

  mov eax, TotalMemMsg
  call Puts32

  ; get and print toal number of available memory
  mov ecx, [boot_info]
  mov eax, [ecx + multiboot_info.memoryLo]
  mov ebx, [ecx + multiboot_info.memoryHi]
  ; xchg bx, bx
  call get_total_memory

  mov [TotalMem], eax
  mov ebx, Buffer
  call NumberToString
  mov eax, Buffer
  call Puts32

  ; get and print total number of blocks
  mov bl, 0
  mov bh, 2
  call MovCur
  mov eax, TotalBlockMsg
  call Puts32
  mov eax, [TotalMem]
  call get_total_block_count

  xchg bx, bx
  mov [mem_block_total], eax
  mov ebx, Buffer
  call NumberToString
  mov eax, Buffer
  call Puts32

  ret

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

; eax: total number of memory
; returns total number of blocks
get_total_block_count:
  ; xchg bx, bxccr

  ; need to clear edx, since edx contains higher dword and eax contains
  ; lower dword, both are combined to be divided by a number
  xor edx, edx
  mov ecx, BLOCK_SIZE
  div ecx

  ret
