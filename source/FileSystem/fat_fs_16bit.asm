; ================================================================
; File System Module for ExMikeOS
; ================================================================

; File system constants
SECTOR_SIZE         equ 512
CLUSTER_SIZE        equ 512 * 1  ; 1 sector per cluster
FAT_SIZE            equ 9         ; Number of sectors for FAT
ROOT_ENTRIES        equ 224       ; Number of root directory entries
ROOT_DIR_SECTORS    equ 14        ; Number of sectors for root directory
DATA_REGION_START   equ 19        ; First sector of data region

; Boot sector structure
struc BootSector
    .jump_code      resb 3
    .oem_name       resb 8
    .bytes_per_sector resw 1
    .sectors_per_cluster resb 1
    .reserved_sectors resw 1
    .fat_count      resb 1
    .root_entries   resw 1
    .total_sectors  resw 1
    .media_type     resb 1
    .sectors_per_fat resw 1
    .sectors_per_track resw 1
    .head_count     resw 1
    .hidden_sectors resd 1
    .large_sector_count resd 1
    .drive_number   resb 1
    .reserved       resb 1
    .signature      resb 1
    .volume_id      resd 1
    .volume_label   resb 11
    .file_system_type resb 8
endstruc

; Root directory entry structure
struc DirEntry
    .filename       resb 8
    .extension      resb 3
    .attributes     resb 1
    .reserved       resb 10      ; Combine reserved fields for simplicity
    .first_cluster_low resw 1
    .file_size      resd 1
endstruc

; ================================================================
; GLOBAL VARIABLES - Adjust as needed for your memory model
; ================================================================

; System drive number
system_drive     db 0x80          ; Default to first hard disk (0x80)

; Global buffer locations
boot_sector     equ 0x7E00        ; Just after MBR if loaded at 0x7C00
fat             equ 0x8000        ; FAT buffer (9 sectors = 4.5KB)
root_dir        equ 0x9000        ; Root directory buffer
file_buffer     equ 0xA000        ; File data buffer

; Working variables
file_cluster    dw 0
current_cluster dw 0
bytes_per_sector dw 512
sectors_per_cluster db 1
sectors_per_track dw 18
heads_per_disk  dw 2

; Status flags
fs_initialized  db 0              ; 0 = not initialized, 1 = initialized

; Error messages
disk_error_msg  db 'Disk read error!', 0
file_not_found_msg db 'File not found!', 0
fs_init_msg     db 'Filesystem initialized', 0

; ================================================================
; FILE SYSTEM FUNCTIONS
; ================================================================

; Print a string (SI = pointer to null-terminated string)
; Uses BIOS teletype function
fs_print_string:
    pusha
    mov ah, 0x0E                ; BIOS teletype function
.loop:
    lodsb                       ; Load byte from SI into AL
    test al, al                 ; Check if we reached the end (0)
    jz .done                    ; If AL=0, we're done
    int 0x10                    ; Print the character
    jmp .loop
.done:
    popa
    ret

; Read a sector from disk
; Input: AX = LBA sector, ES:BX = buffer
; Output: CF set on error
fs_read_sector:
    pusha
    
    ; Calculate CHS from LBA
    xor dx, dx
    div word [sectors_per_track]  ; AX = LBA / SectorsPerTrack, DX = LBA % SectorsPerTrack
    inc dx                        ; Sector = (LBA % SectorsPerTrack) + 1
    mov cx, dx                    ; CL = sector number
    
    xor dx, dx
    div word [heads_per_disk]     ; AX = (LBA / SectorsPerTrack) / NumHeads = Cylinder
                                  ; DX = (LBA / SectorsPerTrack) % NumHeads = Head
    
    mov ch, al                    ; CH = cylinder (lower 8 bits)
    mov dh, dl                    ; DH = head
    
    ; Perform the read
    mov ah, 0x02                  ; BIOS read sector function
    mov al, 1                     ; Read 1 sector
    mov dl, [system_drive]        ; Use system drive
    
    int 0x13                      ; Call BIOS disk service
    jc .error                     ; Carry flag set on error
    
    popa
    clc                           ; Clear carry flag (success)
    ret

.error:
    mov si, disk_error_msg
    call fs_print_string
    popa
    stc                           ; Set carry flag (error)
    ret

; Read multiple sectors
; Input: AX = starting LBA sector, CX = number of sectors, ES:BX = buffer
; Output: CF set on error
fs_read_sectors:
    pusha
    mov di, cx                    ; Save sector count
    
.read_loop:
    call fs_read_sector
    jc .error                     ; Exit if there was an error
    
    add bx, SECTOR_SIZE           ; Move buffer pointer
    inc ax                        ; Next sector
    dec di                        ; Decrease sector count
    jnz .read_loop                ; Continue if we have sectors left to read
    
    popa
    clc                           ; Clear carry flag (success)
    ret
    
.error:
    popa
    stc                           ; Set carry flag (error)
    ret

; Read the boot sector and extract file system parameters
; Output: CF set on error
fs_read_boot_sector:
    pusha
    
    ; Read boot sector (LBA 0)
    push es
    push ds
    pop es                        ; ES = DS
    mov bx, boot_sector           ; Buffer to store boot sector
    mov ax, 0                     ; LBA sector 0
    call fs_read_sector
    pop es
    jc .error
    
    ; Extract file system parameters from boot sector
    mov ax, [boot_sector + BootSector.bytes_per_sector]
    mov [bytes_per_sector], ax
    
    mov al, [boot_sector + BootSector.sectors_per_cluster]
    mov [sectors_per_cluster], al
    
    mov ax, [boot_sector + BootSector.sectors_per_track]
    mov [sectors_per_track], ax
    
    mov ax, [boot_sector + BootSector.head_count]
    mov [heads_per_disk], ax
    
    mov al, [boot_sector + BootSector.drive_number]
    mov [system_drive], al
    
    popa
    clc                           ; Clear carry flag (success)
    ret
    
.error:
    popa
    stc                           ; Set carry flag (error)
    ret

; Read the FAT
; Output: CF set on error
fs_read_fat:
    pusha
    
    ; Read FAT (starting from LBA 1)
    push es
    push ds
    pop es                        ; ES = DS
    mov bx, fat                   ; Buffer to store FAT
    mov ax, 1                     ; Start at LBA sector 1
    mov cx, FAT_SIZE              ; Number of sectors to read
    call fs_read_sectors
    pop es
    jc .error
    
    popa
    clc                           ; Clear carry flag (success)
    ret
    
.error:
    popa
    stc                           ; Set carry flag (error)
    ret

; Read the root directory
; Output: CF set on error
fs_read_root_directory:
    pusha
    
    ; Read root directory
    push es
    push ds
    pop es                        ; ES = DS
    mov bx, root_dir              ; Buffer to store root directory
    mov ax, 1 + FAT_SIZE          ; Start after the FAT
    mov cx, ROOT_DIR_SECTORS      ; Number of sectors to read
    call fs_read_sectors
    pop es
    jc .error
    
    popa
    clc                           ; Clear carry flag (success)
    ret
    
.error:
    popa
    stc                           ; Set carry flag (error)
    ret

; Find a file in the root directory
; Input: SI = file name (8.3 format)
; Output: AX = first cluster, BX = file size, CF = set if not found
fs_find_file:
    pusha
    
    mov cx, ROOT_ENTRIES          ; Number of entries to search
    mov di, root_dir              ; Pointer to root directory

.search_loop:
    push si                       ; Save filename pointer
    push cx                       ; Save entry counter
    push di                       ; Save directory entry pointer
    
    mov cx, 11                    ; Compare 11 characters (8 + 3)
    repe cmpsb                    ; Compare strings
    
    pop di                        ; Restore directory entry pointer
    pop cx                        ; Restore entry counter
    pop si                        ; Restore filename pointer
    
    je .found                     ; Jump if found
    
    add di, 32                    ; Move to next entry (32 bytes per entry)
    loop .search_loop
    
    ; File not found
    popa
    stc                           ; Set carry flag (not found)
    ret

.found:
    ; Get starting cluster and file size from directory entry
    mov ax, [di + 26]             ; Offset for first cluster in directory entry
    mov [esp + 14], ax            ; Store in original AX (for return)
    
    mov ax, [di + 28]             ; File size (lower 16 bits)
    mov [esp + 12], ax            ; Store in original BX (for return)
    
    popa
    clc                           ; Clear carry flag (found)
    ret

; Read cluster chain
; Input: AX = first cluster, ES:BX = buffer
; Output: CF set on error, AX = bytes read
fs_read_cluster_chain:
    pusha
    
    mov [current_cluster], ax     ; Save starting cluster
    xor dx, dx                    ; Initialize bytes read counter
    
.read_loop:
    ; Check if we've reached the end marker or invalid cluster
    cmp word [current_cluster], 0
    je .done
    cmp word [current_cluster], 1
    je .done
    cmp word [current_cluster], 0xFF8
    jae .done
    
    ; Calculate sector for current cluster
    mov ax, [current_cluster]
    sub ax, 2                     ; Clusters start at 2
    movzx cx, byte [sectors_per_cluster]
    mul cx                        ; AX = (cluster - 2) * sectors_per_cluster
    add ax, DATA_REGION_START     ; Add data region offset
    
    ; Read the cluster
    push dx                       ; Save bytes counter
    push es                       ; Save ES:BX
    push bx
    
    movzx cx, byte [sectors_per_cluster]  ; Number of sectors to read
    call fs_read_sectors
    
    pop bx                        ; Restore ES:BX
    pop es
    pop dx                        ; Restore bytes counter
    jc .error
    
    ; Update bytes read counter
    movzx cx, byte [sectors_per_cluster]
    mov ax, [bytes_per_sector]
    mul cx                        ; AX = sectors_per_cluster * bytes_per_sector
    add dx, ax                    ; Add to total bytes read
    
    ; Move buffer pointer for next cluster
    add bx, ax                    ; BX += cluster size
    
    ; Get next cluster from FAT
    mov ax, [current_cluster]
    mov cx, ax
    mov bx, ax
    shr bx, 1                    ; Divide by 2 (12-bit FAT entries)
    add cx, bx                   ; cx = ax + (ax/2) = ax*1.5
    
    mov bx, fat
    add bx, cx                   ; Point to FAT entry
    mov dx, [bx]                 ; Read FAT entry (16 bits)
    
    ; If odd cluster, use upper 12 bits
    test ax, 1
    jz .even_cluster
    
    shr dx, 4                    ; For odd clusters, shift right 4 bits
    jmp .next_cluster

.even_cluster:
    and dx, 0x0FFF               ; For even clusters, mask to 12 bits

.next_cluster:
    mov [current_cluster], dx    ; Save next cluster
    jmp .read_loop               ; Continue with next cluster
    
.done:
    ; Return bytes read in AX
    mov [esp + 14], dx           ; Update original AX with bytes read
    
    popa
    clc                          ; Clear carry flag (success)
    ret
    
.error:
    popa
    stc                          ; Set carry flag (error)
    ret

; ================================================================
; FILE SYSTEM INITIALIZATION
; ================================================================

; Initialize file system
; Output: CF set on error
fs_init:
    pusha
    
    ; Check if already initialized
    cmp byte [fs_initialized], 1
    je .already_init
    
    call fs_read_boot_sector     ; Read boot sector and parameters
    jc .error
    
    call fs_read_fat             ; Read FAT
    jc .error
    
    call fs_read_root_directory  ; Read root directory
    jc .error
    
    ; Mark as initialized
    mov byte [fs_initialized], 1
    
    ; Print initialization message
    mov si, fs_init_msg
    call fs_print_string
    
    popa
    clc                          ; Clear carry flag (success)
    ret
    
.already_init:
    popa
    clc                          ; Already initialized, no error
    ret
    
.error:
    popa
    stc                          ; Set carry flag (error)
    ret

; ================================================================
; FILE SYSTEM PUBLIC API
; ================================================================

; Open a file
; Input: SI = file name (8.3 format)
; Output: AX = first cluster, BX = file size, CF = set if not found
fs_open:
    ; Check if file system is initialized
    cmp byte [fs_initialized], 1
    jne .init_first
    
    ; Find the file
    call fs_find_file
    ret
    
.init_first:
    call fs_init                ; Initialize file system first
    jc .error
    call fs_find_file           ; Then find the file
    ret
    
.error:
    stc                         ; Set carry flag (error)
    ret

; Read a file
; Input: AX = first cluster, ES:BX = buffer
; Output: AX = bytes read, CF set on error
fs_read:
    ; Check if file system is initialized
    cmp byte [fs_initialized], 1
    jne .init_first
    
    ; Read the file cluster chain
    call fs_read_cluster_chain
    ret
    
.init_first:
    push ax                     ; Save cluster number
    push es                     ; Save buffer pointer
    push bx
    
    call fs_init                ; Initialize file system first
    
    pop bx                      ; Restore buffer pointer
    pop es
    pop ax                      ; Restore cluster number
    
    jc .error
    call fs_read_cluster_chain  ; Then read the file
    ret
    
.error:
    stc                         ; Set carry flag (error)
    ret

; Set active drive
; Input: AL = drive number
fs_set_drive:
    mov [system_drive], al
    ret

; Get filesystem status
; Output: AL = 0 if not initialized, 1 if initialized
fs_get_status:
    mov al, [fs_initialized]
    ret