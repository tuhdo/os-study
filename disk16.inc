; al = number of sectors to read
; ch = track to read from
; cl = sector to read
; dh = head number
; dl = drive number
%define num_of_sectors al
%define track_num ch
%define sector_num cl
%define head_num dh
%define drive_num dl
read_sector:
  mov		ah, 0x02			; read floppy sector function
	int		0x13					; call BIOS - Read the sector

  mov ah, 0bh
  mov bh, 00h
  mov bl, 0ffh
  int 0x10
  ret
