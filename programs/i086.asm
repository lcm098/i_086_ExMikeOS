bits 16
%INCLUDE "mikedev.inc"
ORG 32768

start:
    ; Set video mode (40x25 text mode)
    mov ax, 0x0003
    int 0x10

    ; Set background and text color
    call clear_screen

    ; Draw the border
    call draw_border

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
    call console_new_line
    mov si, prompt_msg
    call os_print_string

    ; Check if Esc was pressed (assuming os_input_string sets carry flag on Esc)
    mov ah, 0x00
    int 0x16
    cmp al, 0x1B   ; Check if AL = 27 (Escape key)
    je escape_pressed

    ; Get user input
    mov ax, input_buffer
    mov bx, 1024
    call os_input_string
    call console_new_line
    

    ; Print input buffer (echo the user's input)
    mov si, input_buffer  
    call string_split
    
    mov si, word [array_buffer_ptr] ; Load the pointer to the first substring
    call os_print_string
    call console_new_line

    mov si, word [array_buffer_ptr] ; Load the pointer to the first substring
    call skip_to_next_element
    call os_print_string
    call console_new_line

    mov si, word [array_buffer_ptr] ; Load the pointer to the first substring
    call skip_to_next_element
    call skip_to_next_element
    call os_print_string

    ; Clear the input buffer for the next input
    mov di, input_buffer
    mov cx, 1024
    xor al, al
    rep stosb

    ; Check if the screen needs to be cleared (e.g., after scrolling)
    call check_screen_scroll
    jmp main_loop
    
escape_pressed:
	call os_clear_screen
	ret

; Clear screen and set background color
clear_screen:
    mov ah, 0x06    ; Clear screen
    mov al, 0x00    ; Clear entire window
    mov bh, 0x1F    ; Light blue background, white text
    mov cx, 0x0000  ; Top-left corner
    mov dx, 0x184F  ; Bottom-right corner
    int 0x10
    ret

; Draw border around the screen
draw_border:
    ; Set color attribute for yellow text (0x0E)
    mov ah, 0x09
    mov bh, 0x00    ; Page number
    mov bl, 0x0E    ; Yellow text
    mov cx, 1       ; Number of times to write character

    ; Draw top border
    mov dh, 0       ; Row 0
    mov dl, 0       ; Column 0
.top_loop:
    mov ah, 0x02    ; Set cursor position
    int 0x10
    mov al, '#'     ; Character to write
    mov ah, 0x09    ; Write character with attribute
    int 0x10
    inc dl          ; Move to next column
    cmp dl, 80      ; Check if we reached the end of the row
    jl .top_loop

    ; Draw bottom border
    mov dh, 24      ; Row 24
    mov dl, 0       ; Column 0
.bottom_loop:
    mov ah, 0x02    ; Set cursor position
    int 0x10
    mov al, '#'     ; Character to write
    mov ah, 0x09    ; Write character with attribute
    int 0x10
    inc dl          ; Move to next column
    cmp dl, 80      ; Check if we reached the end of the row
    jl .bottom_loop

    ; Draw left border
    mov dh, 1       ; Row 1
    mov dl, 0       ; Column 0
.left_loop:
    mov ah, 0x02    ; Set cursor position
    int 0x10
    mov al, '#'     ; Character to write
    mov ah, 0x09    ; Write character with attribute
    int 0x10
    inc dh          ; Move to next row
    cmp dh, 24      ; Check if we reached the bottom of the screen
    jl .left_loop

    ; Draw right border
    mov dh, 1       ; Row 1
    mov dl, 79      ; Column 79
.right_loop:
    mov ah, 0x02    ; Set cursor position
    int 0x10
    mov al, '#'     ; Character to write
    mov ah, 0x09    ; Write character with attribute
    int 0x10
    inc dh          ; Move to next row
    cmp dh, 24      ; Check if we reached the bottom of the screen
    jl .right_loop

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

console_new_line:
    ; Get current cursor position
    mov ah, 0x03    ; BIOS function to get cursor position
    mov bh, 0x00    ; Page number (0 for default)
    int 0x10        ; Returns cursor position in DH=row, DL=column

    ; Move to the next line
    inc dh          ; Increment row (move to the next line)
    mov dl, 1       ; Set column to 1 (indent by one space from the left border)

    ; Set new cursor position
    mov ah, 0x02    ; BIOS function to set cursor position
    mov bh, 0x00    ; Page number (0 for default)
    int 0x10        ; Set cursor position

    ret             ; Return from the function


string_split:
    pusha                ; Save all registers
    mov ah, 0x0E         ; BIOS interrupt, function 0x0E (print character)
    mov di, array_buffer ; DI points to the array buffer
    mov bx, 0            ; BX will track the current position in the array buffer

.loop:
    lodsb                ; Load the byte at address DS:SI into AL
    test al, al          ; Check if AL is zero (end of string)
    jz .done             ; If AL is zero, we are done

    cmp al, ' '          ; Check if AL is a space
    je .space            ; If AL is a space, handle it

    cmp al, '"'          ; Check if AL is a double quote
    je .quote            ; If AL is a double quote, handle it

    ; Store the character in the current word buffer
    stosb                ; Store AL at ES:DI and increment DI
    jmp .loop            ; Continue processing the string

.space:
    ; End of a word, store a null terminator
    mov byte [di], 0     ; Null-terminate the current word
    inc di               ; Move DI to the next position in the array buffer
    inc bx               ; Increment the word count
    jmp .loop            ; Continue processing the string

.quote:
    ; Handle text between double quotes
    lodsb                ; Load the next character
    cmp al, '"'          ; Check if AL is a closing double quote
    je .quote_end        ; If AL is a closing double quote, end the quoted text

    ; Store the character in the current word buffer
    stosb                ; Store AL at ES:DI and increment DI
    jmp .quote           ; Continue processing the quoted text

.quote_end:
    ; End of quoted text, store a null terminator
    mov byte [di], 0     ; Null-terminate the current word
    inc di               ; Move DI to the next position in the array buffer
    inc bx               ; Increment the word count
    jmp .loop            ; Continue processing the string

.done:
    ; End of the string, store a null terminator for the last word
    mov byte [di], 0     ; Null-terminate the last word
    inc bx               ; Increment the word count

    ; Return the array buffer
    mov word [array_buffer_ptr], array_buffer ; Store the buffer pointer (16-bit)
    mov word [array_buffer_count], bx         ; Store the word count (16-bit)

    popa                 ; Restore all registers
    ret                  ; Return from the function

skip_to_next_element:
    .skip_loop:
        lodsb                   ; Load the byte at address DS:SI into AL
        test al, al             ; Check if AL is zero (end of substring)
        jnz .skip_loop          ; If not zero, continue skipping
        inc si                  ; Skip the null terminator
    ret

; Data Section
prompt_msg  db '[current dir] -> [/] ', 0
input_buffer times 1024 db 0
welcome_msg db 'ExTerminal [ExMikeOs, ExTerminal v=2025]', 0

; Data
array_buffer times 256 db 0 ; Buffer to store the array of substrings
array_buffer_ptr dw 0       ; Pointer to the array buffer
array_buffer_count dw 0     ; Number of substrings in the array
