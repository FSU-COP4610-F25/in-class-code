.global _start
_start:
	mov $1,%eax
	mov %eax,%edi
	lea m(%rip),%rsi
	mov $13,%dl
	syscall
	mov $60,%al
	dec %edi
	syscall
m:.ascii "Hello, World!"