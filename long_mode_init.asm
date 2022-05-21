global long_mode_start
global read_port
global write_port



section .text
bits 64

extern __start__

%macro PUSHALL 0
    push rax
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
%endmacro

%macro POPALL 0
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rax
%endmacro



long_mode_start:
    ; print `OKAY` to screen
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    
    mov rax, 0x2f592f412f4b2f4f
    mov qword [0xb8000], rax
    
    push rax
    call __start__
   
    hlt


