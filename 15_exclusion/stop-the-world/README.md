**Stop-the-world mutual exclusion**: For applications running on an operating system, disabling interrupts is unacceptable. It would let small bugs or malicious programs disrupt the computer. The OS manages applications because it controls interrupts. Inside the OS kernel, however, disabling interrupts is a common operation.

