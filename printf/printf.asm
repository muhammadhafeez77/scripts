global _start

section .data
    msg: db "Hello World 10 a 3.500000", 10
    msglen: equ $ - msg

section .text
_start:
    ; Write syscall (sys_write = 1)
    mov rax, 1          ; syscall number for write
    mov rdi, 1          ; file descriptor 1 = stdout
    mov rsi, msg        ; pointer to message
    mov rdx, msglen     ; message length
    syscall             ; call Linux kernel

    ; Exit syscall (sys_exit = 60)
    mov rax, 60         ; syscall number for exit
    xor rdi, rdi        ; exit code 0
    syscall             ; call Linux kernel
