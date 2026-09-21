    .global _start
    .text

_start:
    /* TODO: mmap 4096 RW anonymous, write pattern, verify, munmap, exit
       mmap=9 munmap=11 exit=60
       mmap args: rdi=addr rsi=len rdx=prot r10=flags r8=fd r9=off */
    mov $60, %rax
    mov $1, %rdi
    syscall
