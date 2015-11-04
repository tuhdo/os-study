bits 32

%define BLOCK_PER_BYTE 8
%define BLOCK_SIZE 4096

mmap_lbl db 0ah, "Physical memory map: ", 0ah, 0h
mmap_baseAddress_lbl db "Base Address: ", 0h
mmap_length_lbl db "length: ", 0h
mmap_type_lbl db "type: ", 0h
mmap_info_separator db ", ", 0h

mmap_type1_msg db "Available Memory", 0h
mmap_type2_msg db "Reserved", 0h
mmap_type3_msg db "ACPI Reclaim Memory", 0h
mmap_type4_msg db "ACPI NVS Memory", 0h

TotalMemMsg db "Total number of memory: ", 0h
TotalBlockMsg db "Total number of blocks: ", 0h
Buffer times 100 db "0", 0h
TotalMem dd 0
boot_info  dd 0

mem_total dd 0
mem_block_total dd 0
mem_block_used dd 0
; we might support up to 4GB:
; 4GB = 2^32 = 4294967296 bytes
; 4294967296 / 4096 = 1048576 (blocks)
; 1048576 / 8 = 131072 bytes for bit masking all blocks
; since each byte can carry 8 bits corressponding to 8 blocks
; For now, just support 100MB maximum:
mem_block_array times 3200 db 0

%include "mem_info.inc"
; mem_init
mem_init:

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

  mov [mem_block_total], eax
  mov ebx, Buffer
  call NumberToString
  mov eax, Buffer
  call Puts32

  mov bl, 0
  mov bh, 3
  call MovCur
  mov eax, mmap_lbl
  call Puts32
  mov ecx, [boot_info]
  mov eax, [ecx + multiboot_info.mmap_addr]
  mov ebx, [ecx + multiboot_info.mmap_length]
  call report_memory_map

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

  mov [mem_total], eax

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

%macro print_mmap_entry 1
  push edi
  push ecx
  push ebx
  push eax

  mov bl, [_CurX]
  mov bh, cl
  call MovCur

  mov eax, mmap_%1_lbl
  call Puts32

  mov eax, [esp]
  add eax, MemoryMapEntry.%1
  mov eax, [eax]
  mov ebx, Buffer
  call NumberToString

  mov eax, Buffer
  call Puts32

  pop eax
  pop ebx
  pop ecx
  pop edi
%endmacro

; eax: mmap address
; ebx: number of mmap
report_memory_map:
  ; xchg bx, bx
  ; substract ebx by 1
  ; if ebx is 0, quit; else, get next entry
  mov ecx, [_CurY]                    ; y coordinate
.loop:
  ; print relevant information
  mov [_CurX], byte 0
  print_mmap_entry baseAddress
  push eax
  mov eax, mmap_info_separator
  call Puts32
  pop eax
  print_mmap_entry length
  push eax
  mov eax, mmap_info_separator
  call Puts32
  pop eax
  print_mmap_entry type
  inc ecx

  ; get memory map at index i (starting from 0)
  add eax, 24
  dec ebx
  ; xchg bx, bx
  cmp ebx, 0
  je .done
  jmp .loop
.done:
  ret

; eax: block number to be set in bitmap array
mem_set:
  xchg bx, bx
  xor edx, edx
  mov ecx, 8
  div ecx

  mov cl, 7
  sub cl, dl

  mov edx, 1
  shl edx, cl

  or [mem_block_array + eax], dl

  ret

; eax: block number to be cleared in bitmap array
mem_unset:

; eax: block number to be tested in bitmap array
mem_test:

; eax: abosolute address to convert to block
mem_address_to_block:
