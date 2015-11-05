bits 32

%define BLOCKS_PER_BYTE 8
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

block_allocate_msg db "Block allocated at: ", 0h
out_of_mem_msg db "Out of memory", 0h
TotalMemMsg db "Total number of memory: ", 0h
TotalBlockMsg db "Total number of blocks: ", 0h
buffer times 100 db "0", 0h
boot_info  dd 0

mmap_total dd 0
mmap_block_total dd 0
mmap_block_used dd 0
; we might support up to 4GB:
; 4GB = 2^32 = 4294967296 bytes
; 4294967296 / 4096 = 1048576 (blocks)
; 1048576 / 8 = 131072 bytes for bit masking all blocks
; since each byte can carry 8 bits corressponding to 8 blocks
; For now, just support 100MB maximum:
mmap_block_array times 3200 db 0

%include "mem_info.inc"
%include "memory.inc"

; mmap_init
; eax: boot_info
phys_mem_manager_init:
  mov [boot_info], eax
  ; get and print toal number of available memory
  mov ecx, [boot_info]
  mov eax, [ecx + multiboot_info.memoryLo]
  mov ebx, [ecx + multiboot_info.memoryHi]

  ; get total memory and store it
  call get_total_memory
  mov [mmap_total], eax
  call get_total_block_count
  mov [mmap_block_total], eax

  ; print memory info on screen
  call mmap_report

  ; set block 9 in use
  mov eax, 9
  call mmap_set ; becomes 0x40 in mmap_block_array index 1

  ; set block 8 in use
  mov eax, 8
  call mmap_set ; become 0xc0 in mmap_block_array index 1

  ; unset block 9
  mov eax, 9
  call mmap_unset ; become 0x40 in mmap_block_array index 1

  ; test block 8 set
  mov eax, 8
  call mmap_test ; return 1 in eax

  ; test block 8 set with mmap_is_address_in_reserved_block
  ; xchg bx, bx
  mov eax, 32678
  call mmap_is_address_in_reserved_block

  ; test block 9 set with mmap_is_address_in_reserved_block
  mov eax, 36864
  call mmap_is_address_in_reserved_block

  ; set block 0 in use
  ; xchg bx, bx
  mov eax, 0
  call mmap_set ; becomes 0x01 in mmap_block_array index 0

  ; set block 1 in use
  mov eax, 1
  call mmap_set ; becomes 0x03 in mmap_block_array index 0

  ; set block 2 in use
  mov eax, 2
  call mmap_set ; becomes 0x03 in mmap_block_array index 0

  ; test first free block
  ; xchg bx, bx
  call mmap_first_free_block ; should retrieve 3, right next to the first 2 bits above

  ; xchg bx, bx
  ; by default, treat all memory in used
  xor edx, edx
  mov eax, [mmap_block_total]
  mov ecx, BLOCKS_PER_BYTE
  div ecx

  mov ecx, 1
  mov eax, mmap_block_array
  mov ebx, 0xff
  call memset

  ; xchg bx, bx
  call mmap_first_free_block ; should retrieve 10, since first 9 bits are set

  ; test allocating when we can still have free bits in bitmap array
  ; xchg bx, bx
  call mmap_allocate_block

  push eax
  mov bl, 0
  mov bh, [_CurY]
  inc bh
  call MovCur
  mov eax, block_allocate_msg
  call Puts32
  pop eax
  mov ebx, buffer
  call NumberToString
  push eax
  mov eax, buffer
  call Puts32
  pop eax

  ; test freeing allocated memory above
  call mmap_free_block

  ; xchg bx, bx
  ; by default, treat all memory in used
  xor edx, edx
  mov eax, [mmap_block_total]
  mov ecx, BLOCKS_PER_BYTE
  div ecx

  mov ecx, eax
  mov eax, mmap_block_array
  mov ebx, 0xff
  call memset

  ; test freeing allocated memory above
  mov eax, 200
  call mmap_free_block

  ; test when we have no memory to allocate
  call mmap_allocate_block

  ret

; eax: boot_info
mmap_report:
  mov bl, 0
  mov bh, 1
  call MovCur

  mov eax, TotalMemMsg
  call Puts32

  mov eax, [mmap_total]
  mov ebx, buffer
  call NumberToString
  mov eax, buffer
  call Puts32

  ; get and print total number of blocks
  mov bl, 0
  mov bh, 2
  call MovCur
  mov eax, TotalBlockMsg
  call Puts32

  mov eax, [mmap_block_total]
  mov ebx, buffer
  call NumberToString
  mov eax, buffer
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
  mov ebx, buffer
  call NumberToString

  mov eax, buffer
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

%macro find_block_pos 1
  xor edx, edx
  mov ecx, 8
  div ecx

  mov cl, 7
  sub cl, dl

  mov edx, 1

  ; mmap_test doesn't need this shift
  %if %1 == 1
  ; to shift, cl register is a must
  shl edx, cl
  %endif
%endmacro

; eax: block number to be set in bitmap array
mmap_set:
  inc byte [mmap_block_used]

  find_block_pos 1

  or [mmap_block_array + eax], dl

  ret

; eax: block number to be unset
mmap_unset:
  sub byte [mmap_block_used], 1

  find_block_pos 1

  xor [mmap_block_array + eax], dl

  ret

; eax: block number to be checked in bitmap array
mmap_test:
  ; xchg bx, bx
  find_block_pos 0

  mov eax, [mmap_block_array + eax]
  ; to perform a shift, cl register is a must in x86 arch
  shr eax, cl
  and eax, 1

  ret

; returns address of first free block in eax
mmap_first_free_block:
  ; loop the mmap_block_array, find the first bit that is 0
  mov ebx, 0
.loop_byte:
  mov eax, mmap_block_array
  add eax, ebx

  ; start from checking bit 0 in each bitmap byte
  mov ecx, 0
  ; checking bit by bit
.loop_bit:
  mov edx, 0x80
  shr edx, cl
  and edx, [eax]
  inc ecx  ; prepare for next bit after checking
  cmp edx, 0
  je .done_byte
  cmp ecx, 8
  jne .loop_bit
.done_bit:
  inc ebx
  cmp ebx, mmap_block_total
  jne .loop_byte
.done_byte:
  ; ebx holds index in mmap_block_array, store it in eax and calculate
  ; eax * 8
  mov eax, ebx
  mov ebx, BLOCKS_PER_BYTE
  mul ebx

  ; add the offset
  add eax, ecx
  ret

; eax: address to test
; returns 1 if it is, or 0 if not in eax
mmap_is_address_in_reserved_block:
  call mmap_address_to_block
  call mmap_test

  ret

; eax: abosolute address to convert to block
mmap_address_to_block:
  ; divide the memory address by block size
  xor edx, edx
  mov ecx, BLOCK_SIZE
  div ecx

  ret

; returns allocated address in eax
mmap_allocate_block:
  ; check if used blocks are equal to free blocks
  call mmap_is_block_available

  cmp eax, 0
  jle .error
  ; find first free block
  ; xchg bx, bx
  call mmap_first_free_block

  ; set it
  push eax
  call mmap_set
  pop eax

  ; calculate physical address of the block
  mov edx, BLOCK_SIZE
  mul edx

  jmp .done

.error:
  mov bl, 0
  mov bh, [_CurY]
  inc bh
  call MovCur

  push eax
  mov eax, out_of_mem_msg
  call Puts32
  pop eax

.done:
  ret

; eax: address to free block that contains it
mmap_free_block:
  ; xchg bx, bx
  xor edx, edx
  mov ecx, BLOCK_SIZE
  div ecx

  call mmap_unset

  ret

mmap_is_block_available:
  ; xchg bx, bx
  xor edx, edx
  mov eax, [mmap_block_total]
  mov ecx, BLOCKS_PER_BYTE
  div ecx

  cmp edx, 0
  je .loop
  inc eax
.loop:
  sub eax, 1
  mov ebx, mmap_block_array
  mov edx, [ebx + eax]
  and edx, 0xff
  cmp edx, 0xff
  je .loop
.done:
  ret
