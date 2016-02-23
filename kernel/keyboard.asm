%include "stdio32.inc"

%define KBD_ENC_INPUT_BUF 0x60  ; for reading
%define KBD_ENC_CMD_REG 0x60    ; for writing
%define KBD_CTRL_STATUS_REG 0x64  ; for reading
%define KBD_CTRL_CMD_REG 0x64   ; for writing

;; Status register breakdown
%define KBD_CTRL_STATUS_REG_OUT_BUF 0x1
%define KBD_CTRL_STATUS_REG_IN_BUF 0x2
%define KBD_CTRL_STATUS_REG_SYSTEM 0x4
%define KBD_CTRL_STATUS_REG_CMD_DATA 0x8
%define KBD_CTRL_STATUS_REG_LOCKED 0x10
%define KBD_CTRL_STATUS_REG_AUX_BUF 0x20
%define KBD_CTRL_STATUS_REG_TIMEOUT 0x40
%define KBD_CTRL_STATUS_REG_PARITY 2

;;
;; Read keyboard controller status
read_status_kbd_ctrl:
  in al, KBD_CTRL_STATUS_REG
  ret


