.section .text.init
.global _start

_start:
    li sp, 0x00010000
    jal main

end_loop:
    j end_loop