org 0x20000

bits 32

jmp setup_isrs

%include "stdio32.inc"
%include "idt.inc"

setup_isrs:
  pushad
  call InstallIDT
  popad
  retf
