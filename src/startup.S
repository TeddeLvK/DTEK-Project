.section .text.init
.global _start

_start:
    la sp, 0x20004000
    call main

hang:
    j hang

.align 4
.global trap_handler
trap_handler:
    mret