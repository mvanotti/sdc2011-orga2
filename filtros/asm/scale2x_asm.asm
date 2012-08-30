section .data
DEFAULT REL
section .text
global scale2x_asm


; void scale2x_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size)
;src: rdi
;dst: rsi
;h: rdx
;w: rcx
;src_row_size: r8
;dst_row_size: r9

%define src r15
%define dst r14
%define h r13
%define w r12
%define dst_rs r11
%define src_rs r10

%define magic_mask 0x50 ;01010000  ABCD -> AABB
%define magic_mask_1 0xA5 ; 10100101 ABCD -> BBCC
%define magic_mask_2 0xD8 ; 11011000 ABCD -> ACBD

scale2x_asm:
    push rbp
    mov rbp, rsp

    push r15
    push r14
    push r13
    push r12
    push rbx
    push r12

    mov src, rdi
    mov dst, rsi
    mov h, rdx
    mov w, rcx
    mov src_rs, r8
    mov dst_rs, r9

    mov rcx, h
    shr rcx, 1
    mov r8, w
    shr w, 2
    shl w, 3
    shl r8, 62
    shr r8, 62
    
    mov r9, w
    mov rdi, r8
    shl rdi, 1
    

.copio_fila:

    xor rax, rax
    xor rbx, rbx

    .copio_columna:
            movdqu xmm0, [src + rax]
            add src, src_rs
            movdqu xmm1, [src + rax]
            add src, src_rs
            movdqu xmm2, [src + rax]
            sub src, src_rs
            sub src, src_rs

            ;shifteamos para comparar D con F
            movdqa xmm7, xmm1
            psrldq xmm7, 8
            pcmpeqd xmm7, xmm1

            ;comparamos B con H
            movdqa xmm6, xmm0
            pcmpeqd xmm6, xmm2

            ;Hacemos D == F || B == H
            psrldq xmm6, 4
            por xmm7, xmm6
            pshufd xmm7, xmm7, magic_mask

            movdqa xmm6, xmm1
            movdqa xmm3, xmm1

            pshufd xmm0, xmm0, magic_mask_1 ;ABCD -> BBCC
            pshufd xmm2, xmm2, magic_mask_1 ;GHIJ -> HHII
            pshufd xmm3, xmm3, magic_mask_2 ;DEFK -> DFEK
            pshufd xmm6, xmm6, magic_mask_1 ;DEFK -> EEFF Si sale mal queremos dejar este valor.

            movdqa xmm4, xmm3
            pcmpeqd xmm4, xmm0
            movdqa xmm5, xmm3
            pcmpeqd xmm5, xmm2

            movdqa xmm2, xmm7
            pandn xmm7, xmm4
            pandn xmm2, xmm5
            
            movdqa xmm0, xmm3
            movdqa xmm1, xmm3
            
            pand xmm0, xmm7
            pandn xmm7, xmm6
            paddusb xmm0, xmm7

            pand xmm1, xmm2
            pandn xmm2, xmm6
            paddusb xmm1, xmm2

            movdqu [dst + rbx], xmm0
            add dst, dst_rs
            movdqu [dst + rbx], xmm1

            add rbx, 16
            sub dst, dst_rs

            add rax, 8
            cmp rax, r9

            jne .copio_columna
            
        ; process remaining pixels
        cmp r8, 3
        jne .end_line
        
        sub rax, 4
        sub rbx, 8

        movdqu xmm0, [src + rax]
        add src, src_rs
        movdqu xmm1, [src + rax]
        add src, src_rs
        movdqu xmm2, [src + rax]
        sub src, src_rs
        sub src, src_rs

        ;shifteamos para comparar D con F
        movdqa xmm7, xmm1
        psrldq xmm7, 8
        pcmpeqd xmm7, xmm1

        ;comparamos B con H
        movdqa xmm6, xmm0
        pcmpeqd xmm6, xmm2

        ;Hacemos D == F || B == H
        psrldq xmm6, 4
        por xmm7, xmm6
        pshufd xmm7, xmm7, magic_mask

        movdqa xmm6, xmm1
        movdqa xmm3, xmm1

        pshufd xmm0, xmm0, magic_mask_1 ;ABCD -> BBCC
        pshufd xmm2, xmm2, magic_mask_1 ;GHIJ -> HHII
        pshufd xmm3, xmm3, magic_mask_2 ;DEFK -> DFEK
        pshufd xmm6, xmm6, magic_mask_1 ;DEFK -> EEFF Si sale mal queremos dejar este valor.

        movdqa xmm4, xmm3
        pcmpeqd xmm4, xmm0
        movdqa xmm5, xmm3
        pcmpeqd xmm5, xmm2

        movdqa xmm2, xmm7
        pandn xmm7, xmm4
        pandn xmm2, xmm5
        
        movdqa xmm0, xmm3
        movdqa xmm1, xmm3
        
        pand xmm0, xmm7
        pandn xmm7, xmm6
        paddusb xmm0, xmm7

        pand xmm1, xmm2
        pandn xmm2, xmm6
        paddusb xmm1, xmm2

        movdqu [dst + rbx], xmm0
        add dst, dst_rs
        movdqu [dst + rbx], xmm1
        sub dst, dst_rs
        
.end_line
        add src, src_rs
        add dst, dst_rs
        add dst, dst_rs


        sub rcx, 1
        cmp rcx, 0
        jne .copio_fila

    pop r12
    pop rbx
    pop r12
    pop r13
    pop r14
    pop r15

    pop rbp
	ret
