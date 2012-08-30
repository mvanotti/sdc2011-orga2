section .data

magenta_mask: db 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF
DEFAULT REL
section .text
global blit_asm
;void blit_c (unsigned char *src, unsigned char *dst, int h, int w, 
;int src_row_size, int dst_row_size, unsigned char *blit, int bh, 
;int bw, int b_row_size) {

;src rdi, dst rsi, h rdx, w rcx, src_row_size r8, dst_row_size r9, blit RBP + 16, bh RBP + 24, bw RBP + 32, b_row_size RBP + 40


;rax
;rbx
;rcx
;rdx
;rsi
;rdi
;rbp
;r8
;r9
;r10
;r11
;r12
;r13
;r14
;r15

%define src r15
%define dst r14
%define h r13
%define w r12
%define src_rs r11
%define dst_rs r10
%define blit r9
%define b_h r8
%define b_w rdx
%define b_rs rbx
%define i rcx

blit_asm:
    push rbp
    mov rbp, rsp
    push r12
    push r13
    push r14
    push r15
    push rbx

    mov src, rdi
    mov dst, rsi
    mov h, rdx
    mov w, rcx
    mov src_rs, r8
    mov dst_rs, r9

    movdqu xmm2, [magenta_mask]

    mov rsi, w
    imul rsi, 4  ; pixels to bytes

    mov rax, rsi
    xor rdx, rdx
    mov r9, 16
    idiv r9 ; rax queda con la cantidad de pasadas, rdx quedan los bytes que salvan
    mov rdi, rax
    imul rdi, 16
    mov rsi, rdx

    xor rax, rax
    mov eax, [rbp + 24]
    mov b_h, rax

    mov i, h
    sub i, b_h

    .copia_fila:
        xor rax, rax
        .copia_columna:
            movdqa xmm0, [src + rax]
            movdqa [dst + rax], xmm0
            add rax, 16
            cmp rdi, rax
            jne .copia_columna
        ;pixels remaining
        sub rax, 16
        add rax, rsi
        movdqu xmm0, [src + rax]
        movdqu [dst + rax], xmm0

        add src, src_rs
        add dst, dst_rs
        loop .copia_fila

    mov blit, [rbp + 16]

    xor rax, rax
    mov eax, [rbp + 24]
    mov b_h, rax
    mov eax, [rbp + 32]
    mov b_w, rax
    mov eax, [rbp + 40]
    mov b_rs, rax

    
    mov i, b_h
    mov r13, w
    sub r13, b_w
    imul r13, 4
    mov r12, r13
    .copia_fila_blit:
        mov r13, r12
        xor rax, rax
        .copia_columna_no_blit:
            movdqa xmm0, [src + rax]
            movdqa [dst + rax], xmm0
            add rax, 16
            cmp r13, rax
            jg .copia_columna_no_blit
        ;copia_columna_blit_no_alineada
        sub r13, rax
        add rax, r13


        xor r8, r8
        movdqu xmm0, [src + rax]
        movdqa xmm1, [blit]
        ;aplicamos
        movdqa xmm3, xmm2
        pcmpeqd xmm3, xmm1
        pand xmm0, xmm3
        pandn xmm3, xmm1
        por xmm0, xmm3
        movdqu [dst + rax], xmm0
        sub rax, r13
        sub r8, r13

        .copia_columna_blit:
            movdqa xmm0, [src + rax]
            movdqu xmm1, [blit + r8]
            ;aplicamos
            movdqa xmm3, xmm2
            pcmpeqd xmm3, xmm1
            pand xmm0, xmm3
            pandn xmm3, xmm1
            por xmm0, xmm3
            
            movdqa [dst + rax], xmm0
            
            add rax, 16
            add r8, 16
            cmp rdi, rax
            jne .copia_columna_blit

        sub rax, 16
        sub r8, 16
        add r8, rsi
        add rax, rsi
        ; pixels remaining
        movdqu xmm0, [src + rax]
        movdqu xmm1, [blit + r8]
        ; aplicamos
        movdqa xmm3, xmm2
        pcmpeqd xmm3, xmm1
        pand xmm0, xmm3
        pandn xmm3, xmm1
        por xmm0, xmm3
        movdqu [dst + rax], xmm0
            
        add dst, dst_rs
        add src, src_rs
        add blit, b_rs
        dec rcx
        cmp rcx, 0
        jne .copia_fila_blit

fin:
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
	ret
