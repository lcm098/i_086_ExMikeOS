; ------------------------------------------------------------------
; Include file for MikeOS program development (API version 16)
; ------------------------------------------------------------------

; ------------------------------------------------------------------
; IMPORTANT LOCATIONS

os_main			equ	0000h	; Where the OS code starts


; ------------------------------------------------------------------
; COLOURS (eg for os_draw_background and os_draw_block)

%DEFINE BLACK_ON_WHITE		11110000b
%DEFINE WHITE_ON_BLACK		00001111b
%DEFINE WHITE_ON_LIGHT_RED	11001111b
%DEFINE WHITE_ON_GREEN		00101111b
%DEFINE RED_ON_LIGHT_GREEN	10100100b


; ------------------------------------------------------------------
; KEYS

%DEFINE KEY_UP		72
%DEFINE KEY_DOWN	80
%DEFINE KEY_LEFT	75
%DEFINE KEY_RIGHT	77

%DEFINE KEY_ESC		27
%DEFINE KEY_ENTER	13


; ------------------------------------------------------------------
; SYSTEM CALLS

; Screen control

os_clear_screen		equ	0009h	; (Nothing used)
os_print_string		equ	0003h	; SI = zero-terminated string location
os_move_cursor		equ	0006h	; DH, DL = row, column
os_print_horiz_line	equ	000Ch	; AX = 1 for double line, else single
os_print_newline	equ	000Fh	; (Nothing used)
os_draw_background	equ	002Ah	; AX, BX = strings, CX = colour
os_draw_block		equ	00B4h	; BL = colour, DH/DL/SI/DI = pos
os_input_dialog		equ	00A8h	; AX = input string, BX = msg string
os_dialog_box		equ	003Ch	; AX, BX, CX = strings, DX = type
					; OUT: AX = 0 for OK, 1 for cancel
					; (only multi choice for DX = 1)
os_list_dialog		equ	00ABh	; IN: AX = comma-separated list of
					; option strings, BX, CX = help to
					; display at top of list
					; OUT: AX = number (starting from 1)
					; of option selected, or carry if Esc
os_file_selector	equ	005Ah	; Returns filename in AX
os_get_cursor_pos	equ	0069h	; OUT: DH = row, DL = column
os_print_space		equ	006Ch	; (Nothing used)
os_dump_string		equ	006Fh	; SI = string location
os_print_digit		equ	0072h	; AX = digit to print
os_print_1hex		equ	0075h	; Display low nibble of AL in hex
os_print_2hex		equ	0078h	; Display AL in hex
os_print_4hex		equ	007Bh	; Display AX in hex
os_show_cursor		equ	008Ah	; (Nothing used)
os_hide_cursor		equ	008Dh	; (Nothing used)
os_dump_registers	equ	0090h	; Shows contents of AX, BX, CX, DX


; Keyboard handling

os_wait_for_key		equ	0012h	; Returns AL = key pressed
os_check_for_key	equ	0015h	; Returns AL = key pressed


; File handling

os_get_file_list	equ	0042h	; AX = string to store filenames
os_load_file		equ	0021h	; IN: AX = filename string location,
					; CX = location to load file
					; OUT: BX = file size in bytes,
					; carry clear if OK, set if missing
os_write_file		equ	0096h	; AX = filename, BX = data location,
					; CX = number of bytes to save
os_file_exists		equ	0099h	; AX = filename, carry clear if exists
os_create_file		equ	009Ch	; AX = filename
os_remove_file		equ	009Fh	; AX = filename
os_rename_file		equ	00A2h	; AX = filename, BX = new filename
os_get_file_size	equ	00A5h	; AX = filename, OUT: BX = bytes
					; (64K max)


; Sound

os_speaker_tone		equ	001Bh	; AX = note frequency
os_speaker_off		equ	001Eh	; (Nothing used)


; String handling

os_int_to_string	equ	0018h	; AX = unsigned integer, out AX = string
os_sint_to_string	equ	00C0h	; AX = signed integer, out AX = string
os_string_reverse	equ	00AEh	; SI = string location
os_string_length	equ	002Dh	; AX = string loc, returns AX = length
os_string_uppercase	equ	0030h	; AX = zero-terminated string
os_string_lowercase	equ	0033h	; AX = zero-terminated string
os_input_string		equ	0036h	; AX = string location
os_string_copy		equ	0039h	; SI = source, DI = dest strings
os_string_join		equ	003Fh	; AX, BX = source strings, CX = dest
os_string_compare	equ	0045h	; SI, DI = strings, carry set if same
os_string_strincmp	equ	0093h	; As above, but with CL = chars to check
os_string_chomp		equ	0048h	; AX = string location
os_string_strip		equ	004Bh	; SI = string, AX = char to remove
os_string_truncate	equ	004Eh	; SI = string, AX = number of chars
os_set_time_fmt		equ	0084h	; AL = format
os_set_date_fmt		equ	0087h	; AX = format
os_get_time_string	equ	0054h	; BX = string location for eg '20:41'
os_get_date_string	equ	005Dh	; BX = string loc for eg '12/31/2007'
os_find_char_in_string	equ	0066h	; IN: SI = string, AL = char to find
os_long_int_to_string	equ	007Eh	; DX:AX = num, BX = base, DI = string
os_string_to_int	equ	00B1h	; SI = string (up to 65535), AX = int
os_string_charchange	equ	00BAh	; SI = string, AL = old char, BL = new
os_string_parse		equ	00C3h	; SI = string; output: AX/BX/CX/DX =
					; substrings (or 0 if not present)
os_string_tokenize	equ	00CFh	; SI = string, AL = sep char; returns
					; DI = next token


; Math routines

os_get_random		equ	00B7h	; IN: AX, BX = low, high; OUT: CX = num
os_bcd_to_int		equ	0051h	; AL = BCD number, returns AX = integer
os_long_int_negate	equ	0081h	; DX:AX = number


; Ports

os_port_byte_out	equ	00C9h	; IN: DX = port, AL = byte
os_port_byte_in		equ	00CCh	; IN: DX = port; OUT: AL = byte
os_serial_port_enable	equ	00BDh	; AX = 0 for 9600 baud, 1 for 1200
os_send_via_serial	equ	0060h	; AL = byte to send
os_get_via_serial	equ	0063h	; Returns AL = byte received
					; OUT: AX = location, or 0 if not found


; Misc OS functions

os_run_basic		equ	00C6h	; AX = code location in RAM, BX = size,
					; SI = parameter string (or 0 if none)
os_get_api_version	equ	0057h	; Returns API version in AL
os_pause		equ	0024h	; AX = 10ths of second to wait
os_fatal_error		equ	0027h	; AX = error string location
os_create_multiple_files equ 00D2h

; ------------------------------------------------------------------

; ------------------------------------------------------------------
; EXTENDED FUNCTION OPCODES FOR MikeOS
; ------------------------------------------------------------------

; String Operations


os_string_concat              equ 0106h  ; Concatenate two strings
os_string_find_substring      equ 010Ch  ; Find substring within string
os_string_find_char           equ 010Fh  ; Find character in string
os_string_replace_char        equ 0112h  ; Replace character in string
os_string_replace_substring   equ 0115h  ; Replace substring in string
os_string_to_uppercase        equ 0118h  ; Convert string to uppercase
os_string_to_lowercase        equ 011Bh  ; Convert string to lowercase
os_string_trim_whitespace     equ 0121h  ; Remove whitespace from string
os_string_split               equ 0124h  ; Split string by delimiter
os_string_insert              equ 012Ah  ; Insert substring into string
os_string_delete              equ 012Dh  ; Delete portion of string
os_string_pad_left            equ 0130h  ; Pad string from left
os_string_pad_right           equ 0133h  ; Pad string from right
os_string_format              equ 0136h  ; Format string with variables
os_string_starts_with         equ 013Ch  ; Check if string starts with substring
os_string_ends_with           equ 013Fh  ; Check if string ends with substring
os_string_count_char          equ 0142h  ; Count occurrences of character
os_string_count_substring     equ 0145h  ; Count occurrences of substring
os_string_contains            equ 0148h  ; Check if string contains substring
os_string_remove_char         equ 014Bh  ; Remove character from string
os_string_remove_substring    equ 014Eh  ; Remove substring from string
os_string_strip_special_chars equ 0151h  ; Remove special characters
os_string_escape              equ 0154h  ; Escape special characters
os_string_unescape            equ 0157h  ; Unescape special characters
os_string_encode_base64       equ 015Ah  ; Encode string to base64
os_string_decode_base64       equ 015Dh  ; Decode base64 to string
os_string_encode_hex          equ 0160h  ; Encode string to hexadecimal
os_string_decode_hex          equ 0163h  ; Decode hexadecimal to string
os_string_replace_all         equ 0166h  ; Replace all occurrences of substring
os_string_to_title_case       equ 0169h  ; Convert string to title case
os_string_compare_ignore_case equ 016Ch  ; Compare strings ignoring case
os_string_repeat              equ 016Fh  ; Repeat string multiple times
os_string_shuffle             equ 0172h  ; Randomize character order
os_string_swap_case           equ 0175h  ; Swap case of characters
os_string_convert_ascii       equ 0178h  ; Convert between ASCII encodings
os_string_convert_unicode     equ 017Bh  ; Convert between Unicode encodings
os_string_get_hash            equ 017Eh  ; Calculate hash of string
os_string_validate_utf8       equ 0181h  ; Validate UTF-8 encoding
os_string_replace_first       equ 0184h  ; Replace first occurrence
os_string_replace_last        equ 0187h  ; Replace last occurrence
os_string_wrap_text           equ 018Ah  ; Wrap text to specified width
os_string_center_text         equ 018Dh  ; Center text in specified width
os_string_parse_integer       equ 0190h  ; Parse integer from string
os_string_parse_float         equ 0193h  ; Parse float from string
os_string_generate_uuid       equ 0196h  ; Generate a UUID string

; File Operations
os_file_create                equ 0200h  ; Create a new file
os_file_delete                equ 0203h  ; Delete a file
os_file_rename                equ 0206h  ; Rename a file
os_file_copy                  equ 0209h  ; Copy a file
os_file_move                  equ 020Ch  ; Move a file
os_file_open                  equ 020Fh  ; Open a file
os_file_close                 equ 0212h  ; Close a file
os_file_read                  equ 0215h  ; Read from a file
os_file_write                 equ 0218h  ; Write to a file
os_file_append                equ 021Bh  ; Append to a file
os_file_truncate              equ 021Eh  ; Truncate a file
os_file_seek                  equ 0221h  ; Seek within a file
os_file_tell                  equ 0224h  ; Get current position in file
os_file_get_size              equ 0227h  ; Get file size
os_file_get_attributes        equ 022Dh  ; Get file attributes
os_file_set_attributes        equ 0230h  ; Set file attributes
os_file_get_permissions       equ 0233h  ; Get file permissions
os_file_set_permissions       equ 0236h  ; Set file permissions
os_file_get_owner             equ 0239h  ; Get file owner
os_file_set_owner             equ 023Ch  ; Set file owner
os_file_get_creation_time     equ 023Fh  ; Get file creation time
os_file_get_modified_time     equ 0242h  ; Get file modification time
os_file_get_access_time       equ 0245h  ; Get file access time
os_file_set_creation_time     equ 0248h  ; Set file creation time
os_file_set_modified_time     equ 024Bh  ; Set file modification time
os_file_set_access_time       equ 024Eh  ; Set file access time
os_file_lock                  equ 0251h  ; Lock a file
os_file_unlock                equ 0254h  ; Unlock a file
os_file_is_locked             equ 0257h  ; Check if file is locked
os_file_read_line             equ 025Ah  ; Read a line from a file
os_file_write_line            equ 025Dh  ; Write a line to a file
os_file_read_binary           equ 0260h  ; Read binary data from file
os_file_write_binary          equ 0263h  ; Write binary data to file
os_file_find                  equ 0266h  ; Find files matching pattern
os_file_get_extension         equ 0269h  ; Get file extension
os_file_get_filename          equ 026Ch  ; Get filename without path
os_file_get_directory         equ 026Fh  ; Get directory from path
os_file_get_absolute_path     equ 0272h  ; Get absolute path
os_file_get_relative_path     equ 0275h  ; Get relative path
os_file_get_free_space        equ 0278h  ; Get free space on volume
os_file_get_total_space       equ 027Bh  ; Get total space on volume
os_file_is_readonly           equ 027Eh  ; Check if file is readonly
os_file_is_hidden             equ 0281h  ; Check if file is hidden
os_file_is_symlink            equ 0284h  ; Check if file is a symlink
os_file_create_symlink        equ 0287h  ; Create a symbolic link
os_file_create_hardlink       equ 028Ah  ; Create a hard link
os_file_read_symlink          equ 028Dh  ; Read symbolic link target
os_file_get_mime_type         equ 0290h  ; Get MIME type of file
os_file_open_temp             equ 0293h  ; Open temporary file
os_file_write_temp            equ 0296h  ; Write to temporary file
os_file_close_temp            equ 0299h  ; Close temporary file
os_file_encrypt               equ 029Ch  ; Encrypt a file
os_file_decrypt               equ 029Fh  ; Decrypt a file

; Directory Operations
os_directory_create           equ 0300h  ; Create a directory
os_directory_delete           equ 0303h  ; Delete a directory
os_directory_rename           equ 0306h  ; Rename a directory
os_directory_copy             equ 0309h  ; Copy a directory
os_directory_move             equ 030Ch  ; Move a directory
os_directory_list_files       equ 030Fh  ; List files in directory
os_directory_list_subdirectories equ 0312h  ; List subdirectories
os_directory_exists           equ 0315h  ; Check if directory exists
os_directory_get_size         equ 0318h  ; Get directory size
os_directory_get_permissions  equ 031Bh  ; Get directory permissions
os_directory_set_permissions  equ 031Eh  ; Set directory permissions
os_directory_get_owner        equ 0321h  ; Get directory owner
os_directory_set_owner        equ 0324h  ; Set directory owner
os_directory_get_creation_time equ 0327h  ; Get directory creation time
os_directory_get_modified_time equ 032Ah  ; Get directory modification time
os_directory_get_access_time  equ 032Dh  ; Get directory access time
os_directory_set_creation_time equ 0330h  ; Set directory creation time
os_directory_set_modified_time equ 0333h  ; Set directory modification time
os_directory_set_access_time  equ 0336h  ; Set directory access time
os_directory_get_parent       equ 0339h  ; Get parent directory
os_directory_get_absolute_path equ 033Ch  ; Get absolute path
os_directory_get_relative_path equ 033Fh  ; Get relative path
os_directory_is_empty         equ 0342h  ; Check if directory is empty
os_directory_is_hidden        equ 0345h  ; Check if directory is hidden
os_directory_is_symlink       equ 0348h  ; Check if directory is symlink
os_directory_create_symlink   equ 034Bh  ; Create directory symlink
os_directory_create_hardlink  equ 034Eh  ; Create directory hardlink
os_directory_read_symlink     equ 0351h  ; Read symlink target
os_directory_get_mime_type    equ 0354h  ; Get directory MIME type
os_directory_scan             equ 0357h  ; Recursively scan directory
os_directory_search           equ 035Ah  ; Search directory for pattern
os_directory_compare          equ 035Dh  ; Compare two directories
os_directory_backup           equ 0360h  ; Backup directory
os_directory_restore          equ 0363h  ; Restore directory from backup
os_directory_archive          equ 0366h  ; Archive directory
os_directory_extract          equ 0369h  ; Extract directory archive
os_directory_get_metadata     equ 036Ch  ; Get directory metadata
os_directory_set_metadata     equ 036Fh  ; Set directory metadata
os_directory_lock             equ 0372h  ; Lock directory
os_directory_unlock           equ 0375h  ; Unlock directory
os_directory_is_locked        equ 0378h  ; Check if directory is locked
os_directory_get_disk_usage   equ 037Bh  ; Get disk usage stats
os_directory_set_disk_quota   equ 037Eh  ; Set disk quota
os_directory_check_integrity  equ 0381h  ; Check directory integrity
os_directory_repair           equ 0384h  ; Repair directory structure

; Disk Operations
os_disk_initialize            equ 0400h  ; Initialize disk subsystem
os_disk_shutdown              equ 0403h  ; Shutdown disk subsystem
os_disk_format                equ 0406h  ; Format disk
os_disk_partition_create      equ 0409h  ; Create partition
os_disk_partition_delete      equ 040Ch  ; Delete partition
os_disk_partition_resize      equ 040Fh  ; Resize partition
os_disk_partition_list        equ 0412h  ; List partitions
os_disk_mount                 equ 0415h  ; Mount disk
os_disk_unmount               equ 0418h  ; Unmount disk
os_disk_get_free_space        equ 041Bh  ; Get free space
os_disk_get_total_space       equ 041Eh  ; Get total space
os_disk_get_used_space        equ 0421h  ; Get used space
os_disk_check_health          equ 0424h  ; Check disk health
os_disk_repair                equ 0427h  ; Repair disk
os_disk_defragment            equ 042Ah  ; Defragment disk
os_disk_optimize              equ 042Dh  ; Optimize disk
os_disk_encrypt               equ 0430h  ; Encrypt disk
os_disk_decrypt               equ 0433h  ; Decrypt disk
os_disk_backup                equ 0436h  ; Backup disk
os_disk_restore               equ 0439h  ; Restore disk
os_disk_clone                 equ 043Ch  ; Clone disk
os_disk_wipe                  equ 043Fh  ; Wipe disk
os_disk_scan_for_errors       equ 0442h  ; Scan for errors
os_disk_get_serial_number     equ 0445h  ; Get disk serial number
os_disk_get_model_info        equ 0448h  ; Get disk model info
os_disk_get_firmware_version  equ 044Bh  ; Get firmware version
os_disk_get_temperature       equ 044Eh  ; Get disk temperature
os_disk_get_smart_status      equ 0451h  ; Get SMART status
os_disk_enable_smart_monitoring equ 0454h  ; Enable SMART monitoring
os_disk_disable_smart_monitoring equ 0457h  ; Disable SMART monitoring
os_disk_get_io_stats          equ 045Ah  ; Get I/O statistics
os_disk_set_read_speed        equ 045Dh  ; Set read speed
os_disk_set_write_speed       equ 0460h  ; Set write speed
os_disk_get_read_speed        equ 0463h  ; Get read speed
os_disk_get_write_speed       equ 0466h  ; Get write speed
os_disk_create_virtual_disk   equ 0469h  ; Create virtual disk
os_disk_attach_virtual_disk   equ 046Ch  ; Attach virtual disk
os_disk_detach_virtual_disk   equ 046Fh  ; Detach virtual disk
os_disk_create_snapshot       equ 0472h  ; Create disk snapshot
os_disk_restore_snapshot      equ 0475h  ; Restore from snapshot
os_disk_expand_storage        equ 0478h  ; Expand storage
os_disk_shrink_storage        equ 047Bh  ; Shrink storage
os_disk_compress              equ 047Eh  ; Compress disk
os_disk_decompress            equ 0481h  ; Decompress disk
os_disk_encrypt_partition     equ 0484h  ; Encrypt partition
os_disk_decrypt_partition     equ 0487h  ; Decrypt partition
os_disk_migrate_data          equ 048Ah  ; Migrate data
os_disk_set_cache_policy      equ 048Dh  ; Set cache policy
os_disk_get_cache_policy      equ 0490h  ; Get cache policy

; Graphics Operations
os_graphics_init              equ 0500h  ; Initialize graphics
os_graphics_shutdown          equ 0503h  ; Shutdown graphics
os_graphics_clear_screen      equ 0506h  ; Clear screen
os_graphics_set_resolution    equ 0509h  ; Set resolution
os_graphics_get_resolution    equ 050Ch  ; Get resolution
os_graphics_set_color_mode    equ 050Fh  ; Set color mode
os_graphics_get_color_mode    equ 0512h  ; Get color mode
os_graphics_enable_vsync      equ 0515h  ; Enable vsync
os_graphics_disable_vsync     equ 0518h  ; Disable vsync
os_graphics_set_refresh_rate  equ 051Bh  ; Set refresh rate
os_graphics_draw_pixel        equ 051Eh  ; Draw pixel
os_graphics_get_pixel         equ 0521h  ; Get pixel color
os_graphics_draw_line         equ 0524h  ; Draw line
os_graphics_draw_rectangle    equ 0527h  ; Draw rectangle
os_graphics_fill_rectangle    equ 052Ah  ; Fill rectangle
os_graphics_draw_circle       equ 052Dh  ; Draw circle
os_graphics_fill_circle       equ 0530h  ; Fill circle
os_graphics_draw_ellipse      equ 0533h  ; Draw ellipse
os_graphics_fill_ellipse      equ 0536h  ; Fill ellipse
os_graphics_draw_polygon      equ 0539h  ; Draw polygon
os_graphics_fill_polygon      equ 053Ch  ; Fill polygon
os_graphics_draw_arc          equ 053Fh  ; Draw arc
os_graphics_draw_bezier       equ 0542h  ; Draw bezier curve
os_graphics_draw_gradient_rect equ 0545h  ; Draw gradient rectangle
os_graphics_draw_triangle     equ 0548h  ; Draw triangle
os_graphics_fill_triangle     equ 054Bh  ; Fill triangle
os_graphics_draw_shadow       equ 054Eh  ; Draw shadow
os_graphics_draw_texture      equ 0551h  ; Draw texture
os_graphics_draw_alpha_blend  equ 0554h  ; Draw with alpha blending
os_graphics_draw_antialiased_line equ 0557h  ; Draw antialiased line
os_graphics_draw_text         equ 055Ah  ; Draw text
os_graphics_set_font          equ 055Dh  ; Set font
os_graphics_get_font          equ 0560h  ; Get current font
os_graphics_set_text_size     equ 0563h  ; Set text size
os_graphics_get_text_size     equ 0566h  ; Get text size
os_graphics_set_text_color    equ 0569h  ; Set text color
os_graphics_get_text_color    equ 056Ch  ; Get text color
os_graphics_draw_text_box     equ 056Fh  ; Draw text in box
os_graphics_measure_text      equ 0572h  ; Measure text dimensions
os_graphics_draw_rich_text    equ 0575h  ; Draw rich text
os_graphics_load_image        equ 0578h  ; Load image
os_graphics_draw_image        equ 057Bh  ; Draw image
os_graphics_resize_image      equ 057Eh  ; Resize image
os_graphics_rotate_image      equ 0581h  ; Rotate image
os_graphics_flip_image        equ 0584h  ; Flip image
os_graphics_crop_image        equ 0587h  ; Crop image
os_graphics_save_image        equ 058Ah  ; Save image
os_graphics_create_sprite     equ 058Dh  ; Create sprite
os_graphics_draw_sprite       equ 0590h  ; Draw sprite
os_graphics_animate_sprite    equ 0593h  ; Animate sprite
os_graphics_enable_double_buffering equ 0596h  ; Enable double buffering
os_graphics_disable_double_buffering equ 0599h  ; Disable double buffering
os_graphics_swap_buffers      equ 059Ch  ; Swap buffers
os_graphics_create_framebuffer equ 059Fh  ; Create framebuffer
os_graphics_bind_framebuffer  equ 05A2h  ; Bind framebuffer
os_graphics_clear_framebuffer equ 05A5h  ; Clear framebuffer
os_graphics_draw_framebuffer  equ 05A8h  ; Draw framebuffer
os_graphics_get_framebuffer_size equ 05ABh  ; Get framebuffer size
os_graphics_enable_alpha_blending equ 05AEh  ; Enable alpha blending
os_graphics_disable_alpha_blending equ 05B1h  ; Disable alpha blending


; Socket & Networking Operations
os_socket_create              equ 0600h  ; Create socket
os_socket_close               equ 0603h  ; Close socket
os_socket_bind                equ 0606h  ; Bind socket
os_socket_listen              equ 0609h  ; Listen for connections
os_socket_accept              equ 060Ch  ; Accept connection
os_socket_connect             equ 060Fh  ; Connect to server
os_socket_send                equ 0612h  ; Send data
os_socket_receive             equ 0615h  ; Receive data
os_socket_sendto              equ 0618h  ; Send to specific address
os_socket_receivefrom         equ 061Bh  ; Receive from address
os_socket_shutdown            equ 061Eh  ; Shutdown socket
os_socket_set_option          equ 0621h  ; Set socket option
os_socket_get_option          equ 0624h  ; Get socket option
os_socket_set_blocking        equ 0627h  ; Set socket to blocking
os_socket_set_nonblocking     equ 062Ah  ; Set socket to non-blocking
os_socket_get_status          equ 062Dh  ; Get socket status
os_socket_get_error           equ 0630h  ; Get socket error
os_socket_clear_error         equ 0633h  ; Clear socket error
os_socket_get_peer_address    equ 0636h  ; Get peer address
os_socket_get_local_address   equ 0639h  ; Get local address
os_socket_get_protocol        equ 063Ch  ; Get socket protocol
os_socket_set_timeout         equ 063Fh  ; Set socket timeout
os_socket_get_timeout         equ 0642h  ; Get socket timeout
os_socket_poll                equ 0645h  ; Poll socket
os_socket_select              equ 0648h  ; Select multiple sockets
os_socket_epoll_create        equ 064Bh  ; Create epoll instance
os_socket_epoll_ctl           equ 064Eh  ; Control epoll
os_socket_epoll_wait          equ 0651h  ; Wait for epoll events
os_socket_get_hostname        equ 0654h  ; Get hostname
os_socket_get_host_by_name    equ 0657h  ; Get host by name
os_socket_get_host_by_address equ 065Ah  ; Get host by address
os_socket_get_service_by_name equ 065Dh  ; Get service by name
os_socket_get_service_by_port equ 0660h  ; Get service by port
os_socket_get_interface_list  equ 0663h  ; Get network interfaces
os_socket_get_interface_address equ 0666h  ; Get interface address
os_socket_set_interface       equ 0669h  ; Set network interface
os_socket_enable_broadcast    equ 066Ch  ; Enable broadcasting
os_socket_disable_broadcast   equ 066Fh  ; Disable broadcasting
os_socket_enable_multicast    equ 0672h  ; Enable multicast
os_socket_disable_multicast   equ 0675h  ; Disable multicast
os_socket_join_multicast_group equ 0678h  ; Join multicast group
os_socket_leave_multicast_group equ 067Bh  ; Leave multicast group
os_socket_set_ttl             equ 067Eh  ; Set TTL
os_socket_get_ttl             equ 0681h  ; Get TTL
os_socket_enable_keepalive    equ 0684h  ; Enable keepalive
os_socket_disable_keepalive   equ 0687h  ; Disable keepalive
os_socket_set_keepalive_interval equ 068Ah  ; Set keepalive interval
os_socket_get_keepalive_interval equ 068Dh  ; Get keepalive interval
os_socket_enable_nagle_algorithm equ 0690h  ; Enable Nagle algorithm
os_socket_disable_nagle_algorithm equ 0693h  ; Disable Nagle algorithm
os_socket_get_send_buffer_size equ 0696h  ; Get send buffer size
os_socket_set_send_buffer_size equ 0699h  ; Set send buffer size
os_socket_get_receive_buffer_size equ 069Ch  ; Get receive buffer size
os_socket_set_receive_buffer_size equ 069Fh  ; Set receive buffer size
os_socket_create_raw          equ 06A2h  ; Create raw socket
os_socket_send_raw            equ 06A5h  ; Send raw packet
os_socket_receive_raw         equ 06A8h  ; Receive raw packet
os_socket_create_secure       equ 06ABh  ; Create secure socket
os_socket_set_certificate     equ 06AEh  ; Set SSL certificate
os_socket_verify_certificate  equ 06B1h  ; Verify SSL certificate
os_socket_enable_encryption   equ 06B4h  ; Enable encryption
os_socket_disable_encryption  equ 06B7h  ; Disable encryption
os_socket_dns_lookup          equ 06BAh  ; Perform DNS lookup
os_socket_ping_host           equ 06BDh  ; Ping remote host
os_socket_traceroute          equ 06C0h  ; Trace route to host
os_socket_get_network_statistics equ 06C3h  ; Get network statistics
os_socket_enable_qos          equ 06C6h  ; Enable QoS
os_socket_disable_qos         equ 06C9h  ; Disable QoS
os_socket_set_qos_priority    equ 06CCh  ; Set QoS priority
os_socket_get_qos_priority    equ 06CFh  ; Get QoS priority
os_socket_set_max_connections equ 06D2h  ; Set max connections
os_socket_get_max_connections equ 06D5h  ; Get max connections
