bits 16
%INCLUDE "mikedev.inc"
ORG 32768

start:
    ; Set video mode (40x25 text mode)
    mov ax, 0x0003
    int 0x10

    ; Set background and text color
    call clear_screen

    ; Set cursor position
    mov ah, 0x02
    mov bh, 0x00    ; Page number
    mov dh, 1       ; Row
    mov dl, 1       ; Column
    int 0x10
    mov si, welcome_msg
    call os_print_string

    ; Main loop
main_loop:
    ; Print prompt message
    call os_print_newline
    mov si, prompt_msg
    call os_print_string

    ; Get user input
    mov ax, input_buffer
    mov bx, 1024
    call os_input_string
    call os_print_newline

    ; Print input buffer (echo the user's input)
    mov si, input_buffer  
    call os_print_string  ; Print what the user typed

    ; Clear the input buffer for the next input
    mov di, input_buffer
    mov cx, 1024
    xor al, al
    rep stosb

    ; Check if the screen needs to be cleared (e.g., after scrolling)
    call check_screen_scroll

    jmp main_loop

; Clear screen and set background color
clear_screen:
    mov ah, 0x06    ; Clear screen
    mov al, 0x00    ; Clear entire window
    mov bh, 0x1F    ; Light blue background, white text
    mov cx, 0x0000  ; Top-left corner
    mov dx, 0x184F  ; Bottom-right corner
    int 0x10
    ret

; Check if the screen has scrolled and clear it if necessary
check_screen_scroll:
    ; Get current cursor position
    mov ah, 0x03
    mov bh, 0x00    ; Page number
    int 0x10        ; Returns cursor position in DH=row, DL=column

    ; Check if the cursor is at the bottom of the screen
    cmp dh, 24
    jl .done        ; If not at the bottom, no need to clear the screen

    ; If at the bottom, clear the screen and reset the background
    call clear_screen

    ; Reset cursor position to the top of the screen
    mov ah, 0x02
    mov bh, 0x00
    mov dh, 1       ; Row
    mov dl, 1       ; Column
    int 0x10

.done:
    ret

; Data Section
prompt_msg  db '[current dir] -> [/] ', 0
input_buffer times 1024 db 0
welcome_msg db 'ExTerminal [ExMikeOs, ExTerminal v=2025]', 0

; Boot sector padding (if needed)
; times 510-($-$$) db 0
; dw 0xAA55