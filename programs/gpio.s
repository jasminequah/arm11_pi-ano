ldr r0 =0x20200004
mov r6, #0x00040000
str r6, [r0]
mov r1, #0x00010000
ldr r2, =0x20200028
str r1, [r2]
ldr r3, =0x2020001C
str r1, [r3]
mov r5, #100
mainloop:
mov r4, #0xff
loop:
str r1, [r3]
sub r4, r4, #1
cmp r4, #0
blt loop
str r1, [r2]
mov r4, #0xff
loop2:
str r1, [r2]
sub r4, r4, #1
cmp r4, #0
blt loop2
sub r5, r5, #1
cmp r5, #0
blt mainloop
