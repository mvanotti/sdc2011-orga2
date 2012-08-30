section .data
DEFAULT REL

section .text
global sepia_asm
coeficientes: dd 0.2, 0.3, 0.5, 0
%define SHUFFLE_MASK 0xFF
%define ROTATE_LEFT_MASK 10010011

sepia_asm:

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
    movdqu xmm0, [coeficientes]

    pxor xmm1, xmm1
.loop:

            movdqa xmm2, [rdi + r11]  ; Muevo 16 bytes alineados y proceso
            movdqa xmm4, xmm2
            punpcklbw xmm2, xmm1
            punpckhbw xmm4, xmm1
            phaddw xmm2, xmm4

            movdqa xmm4, xmm2
            punpcklwd xmm2, xmm1
            punpckhwd xmm4, xmm1
            phaddd xmm2, xmm4
            cvtdq2ps xmm2, xmm2
            movdqa xmm6, xmm2
            shufps xmm2, xmm6, 0x00    
            shufps xmm3, xmm6, 0x40
            shufps xmm4, xmm6, 0x80
            shufps xmm5, xmm6, 0xc0
            pshufd xmm2, xmm2, SHUFFLE_MASK
            pshufd xmm3, xmm3, SHUFFLE_MASK
            pshufd xmm4, xmm4, SHUFFLE_MASK
            pshufd xmm5, xmm5, SHUFFLE_MASK

            mulps xmm2, xmm0
            mulps xmm3, xmm0
            mulps xmm4, xmm0
            mulps xmm5, xmm0

            cvtps2dq xmm2, xmm2
            cvtps2dq xmm3, xmm3
            cvtps2dq xmm4, xmm4
            cvtps2dq xmm5, xmm5
            packusdw xmm4, xmm5
            packusdw xmm2, xmm3
            packuswb xmm2, xmm4
            movdqa [rsi + r11], xmm2
            add r11, 16
            cmp r11, r10
            jne .loop
        ;process remaining pixels
        sub r11, 16 ;Para procesar la última tira de 16 bytes, resto 16 bytes y sumo lo que me falta procesar       
        add r11, rdx
        movdqu xmm2, [rdi + r11]  ; Muevo 16 bytes alineados y proceso
        movdqa xmm4, xmm2
        punpcklbw xmm2, xmm1
        punpckhbw xmm4, xmm1
        phaddw xmm2, xmm4

        movdqa xmm4, xmm2
        punpcklwd xmm2, xmm1
        punpckhwd xmm4, xmm1
        phaddd xmm2, xmm4
        cvtdq2ps xmm2, xmm2
        movdqa xmm6, xmm2
        shufps xmm2, xmm6, 0x00    
        shufps xmm3, xmm6, 0x40
        shufps xmm4, xmm6, 0x80
        shufps xmm5, xmm6, 0xc0
        pshufd xmm2, xmm2, SHUFFLE_MASK
        pshufd xmm3, xmm3, SHUFFLE_MASK
        pshufd xmm4, xmm4, SHUFFLE_MASK
        pshufd xmm5, xmm5, SHUFFLE_MASK

        mulps xmm2, xmm0
        mulps xmm3, xmm0
        mulps xmm4, xmm0
        mulps xmm5, xmm0

        cvtps2dq xmm2, xmm2
        cvtps2dq xmm3, xmm3
        cvtps2dq xmm4, xmm4
        cvtps2dq xmm5, xmm5
        packusdw xmm4, xmm5
        packusdw xmm2, xmm3
        packuswb xmm2, xmm4
        movdqu [rsi + r11], xmm2
        
        xor r11, r11
        add rsi, r9
        add rdi, r8
	dec rcx
	cmp rcx, 0
    jne .loop
	ret
