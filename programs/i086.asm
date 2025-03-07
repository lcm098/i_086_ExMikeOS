bits 16
%INCLUDE "mikedev.inc"
ORG 32768

start:
    ; Set video mode (80x25 text mode)
    mov ax, 0x0002
    int 0x10

    ; Set background and text color
    mov ah, 0x06    ; Clear screen
    mov al, 0x00    ; Clear entire window
    mov bh, 0x1F    ; Light blue background, white text
    mov cx, 0x0000  ; Top-left corner
    mov dx, 0x184F  ; Bottom-right corner
    int 0x10


    ; Set cursor position
    mov ah, 0x02
    mov bh, 0x00    ; Page number
    mov dh, 1       ; Row
    mov dl, 1       ; Column
    int 0x10

    ; Main loop
main_loop:
    
    ; Print welcome message
    call os_print_newline ; Move to the next line
    mov si, prompt_msg
    call os_print_string

    mov ax, input_buffer
    mov bx, 1024
    call os_input_string
    call os_print_newline

    ; Print input buffer
    mov si, input_buffer  
    call os_print_string  ; Print what the user typed

    jmp main_loop


; Data Section
prompt_msg  db '[current dir] -> [/] ', 0
input_buffer times 1024 db 0

; Boot sector padding (if needed)
; times 510-($-$$) db 0
; dw 0xAA55