section .data
    message db "Hello, World", 0xA ; The message to print with a newline character
    msg_len equ $ - message         ; Length of the message

section .text
    global _start

_start:
    ; Write system call
    mov rax, 1          ; Syscall number for sys_write (1)
    mov rdi, 1          ; File descriptor for stdout (1)
    mov rsi, message    ; Pointer to the message
    mov rdx, msg_len    ; Length of the message
    syscall             ; Invoke the system call

    ; Exit system call
    mov rax, 60         ; Syscall number for sys_exit (60)
    xor rdi, rdi        ; Exit status (0)
    syscall             ; Invoke the system call