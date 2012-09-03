section .data
invert_mask: db 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
DEFAULT REL

section .text
global rotar_asm
;rotar_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size)
;                       rdi                 rsi      rdx     rcx            r8          r9


rotar_asm:
    xor r11, r11
    mov r10, rcx ; w
    mov rcx, rdx ; h (loop)
    imul r10, 4  ; pixels to bytes

    mov rax, r10
    xor rdx, rdx
    mov r10, 16
    idiv r10 ; En rax dejo la cantidad de pasadas, y en rdx los bytes que me quedan por procesar
    mov r10, rax
    imul r10, 16
    movdqu xmm0, [invert_mask]
.loop:
            movdqa xmm1, [rdi + r11]  ; Muevo 16 bytes alineados y aplico la mascara para rotarlos
            psubusb xmm0, xmm1
            movdqa xmm1, xmm0
            movdqa [rsi + r11], xmm1
            add r11, 16
            cmp r11, r10
            jne .loop
        ;process remaining pixels
        add r11, rdx  ; Para procesar la última tira de 16 bytes, resto 16 bytes y sumo lo que me falta procesar
        sub r11, 16
        movdqu xmm1, [rdi + r11]
        psubusb xmm0, xmm1
        movdqa xmm1, xmm0
        movdqu [rsi + r11], xmm1

        xor r11, r11
        add rsi, r9
        add rdi, r8
        loop .loop
	ret
