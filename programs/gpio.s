ldr r0, =0x20200004
ldr r6, =0x00040000
str r6, [r0]
ldr r1, =0x00010000
ldr r2, =0x20200028
str r1, [r2]
ldr r3, =0x2020001C
mainloop:
ldr r4, =0xabeefa
str r1, [r3]
loop:
sub r4, r4, #1
cmp r4, #0
bne loop
str r6, [r2]
ldr r4, =0xabeefa
loop2:
sub r4, r4, #1
cmp r4, #0
bne loop2
b mainloop
