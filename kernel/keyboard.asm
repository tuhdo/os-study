%include "stdio32.inc"
%include "a20.inc"

%define KBD_ENC_INPUT_BUF_REG 0x60  ; for reading
%define KBD_ENC_CMD_REG 0x60    ; for writing
%define KBD_CTRL_STATUS_REG 0x64  ; for reading
%define KBD_CTRL_CMD_REG 0x64   ; for writing

;; Keyboard encoder commands
%define CMD_KBD_ENC_SET_LEDS 0xED ; SET LEDs
%define CMD_KBD_ENC_ECHO 0xEE ; 0xEE	Echo command. Returns 0xEE to port 0x60 as a diagnostic test
%define CMD_KBD_ENC_SET_ALT_SCAN_CODE 0xF0 ; Set alternate scan code set
%define CMD_KBD_ENC_SEND_2B_KBD_ID 0xF2 ; Send 2 byte keyboard ID code as the next two bytes to be read from port 0x60
%define CMD_KBD_ENC_SET_AUTO_DELAY_AND_REPEAT_RATE 0xF3 ;	Set autrepeat delay and repeat rate
%define CMD_KBD_ENC_ENABLE_KEYBOARD 0xF4 ; Enable keyboard
%define CMD_KBD_ENC_RESET_WAIT_ENABLE 0xF5 ;	Reset to power on condition and wait for enable command
%define CMD_KBD_ENC_RESET_NO_WAIT 0xF6 ; Reset to power on condition and begin scanning keyboard
%define CMD_KBD_ENC_SET_AUTO_PS2 0xF7 ;	Set all keys to autorepeat (PS/2 only)
%define CMD_KBD_ENC_SET_ALL_MAKE_BREAK 0xF8 ;	Set all keys to send make code and break code (PS/2 only)
%define CMD_KBD_ENC_SET_ALL_ONLY_MAKE 0xF9 ; Set all keys to generate only make codes
%define CMD_KBD_ENC_SET_AUTO_MAKE_BREAK 0xFA ; Set all keys to autorepeat and generate make/break codes
%define CMD_KBD_ENC_SET_SINGLE_KEY_AUTO 0xFB ; Set a single key to autorepeat
%define CMD_KBD_ENC_SET_SINGLE_KEY_MAKE_BREAK 0xFC ; Set a single key to generate make and break codes
%define CMD_KBD_ENC_SET_SINGLE_KEY_BREAK 0xFD ; Set a single key to generate only break codes
%define CMD_KBD_ENC_RESEND 0xED ; Resend last result
%define CMD_KBD_ENC_RESET_AND_TEST 0xED ; Reset keyboard to power on state and start self test

;; Status register breakdown
%define MASK_KBD_CTRL_STATUS_REG_OUT_BUF 0x1
%define MASK_KBD_CTRL_STATUS_REG_IN_BUF 0x2
%define MASK_KBD_CTRL_STATUS_REG_SYSTEM 0x4
%define MASK_KBD_CTRL_STATUS_REG_CMD_DATA 0x8
%define MASK_KBD_CTRL_STATUS_REG_LOCKED 0x10
%define MASK_KBD_CTRL_STATUS_REG_AUX_BUF 0x20
%define MASK_KBD_CTRL_STATUS_REG_TIMEOUT 0x40
%define MASK_KBD_CTRL_STATUS_REG_PARITY 2

;; keyboard controller commands
%define CMD_KBD_CTRL_READ_COMMANd 0x20 ; Read command byte
%define CMD_KBD_CTRL_WRITE_COMMAND 0x60 ; Write command byte
%define CMD_KBD_CTRL_SELF_TEST 0xAA ; Self-test
%define CMD_KBD_CTRL_INTERFACE_TEST 0xAB ; Interface test
%define CMD_KBD_CTRL_DISABLE_KEYBOARD 0xAD ; Disable keyboard
%define CMD_KBD_CTRL_ENABLE_KEYBOARD 0xAE ; Enable keyboard
%define CMD_KBD_CTRL_READ_INPUT_PORT 0xC0 ; Read Input Port
%define CMD_KBD_CTRL_READ_OUTPUT_PORT 0xD0 ; Read Output Port
%define CMD_KBD_CTRL_WRITE_OUTPUT_PORT 0xD1 ; Write Output Port
%define CMD_KBD_CTRL_READ_TEST_INPUTS 0xE0 ; Read Test Inputs
%define CMD_KBD_CTRL_SYSTEM_RESET 0xFE ; System Reset
%define CMD_KBD_CTRL_DISABLE_MOUSE_PORT 0xA7 ; Disable Mouse Port
%define CMD_KBD_CTRL_ENABLE_MOUSE_PORT 0xA8 ; Enable Mouse Port
%define CMD_KBD_CTRL_TEST_MOUSE_PORT 0xA9 ; Test Mouse Port
%define CMD_KBD_CTRL_WRITE_TO_MOUSE 0xD4 ; Write To Mouse

;;
;; Read keyboard controller status
read_status_kbd_ctrl:
  in al, MASK_KBD_CTRL_STATUS_REG
  ret

;; Read in stored in al
read_kbd_enc_buffer:
  in al, KBD_ENC_INPUT_BUF_REG
  ret

;; Run keyboard enconder command stored in al
run_kbd_enc_cmd:
  out al, KBD_ENC_CMD_REG
  ret
