section .data
DEFAULT REL
;	.long	1032805417 ;0.07f
;	.long	1060487823 ;0.71f
;	.long	1045891645 ;0.21f
coeficientes: dd 0.21, 0.71, 0.07, 0
pshufb_mask: db 0, 0, 0, 0xF, 4, 4, 4, 0xF, 8, 8, 8, 0xF, 0xC, 0xC, 0xC, 0xF
section .text
global monocromatizar_asm
monocromatizar_asm:
;COMPLETAR
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
    movdqu xmm6, [pshufb_mask]
            pxor xmm1, xmm1
.loop:
            movdqa xmm2, [rdi + r11]  ; Muevo 16 bytes alineados y proceso
            movdqa xmm4, xmm2
            punpcklbw xmm2, xmm1
            punpckhbw xmm4, xmm1
            movdqa xmm3, xmm2
            movdqa xmm5, xmm4
            punpcklwd xmm2, xmm1
            punpckhwd xmm3, xmm1
            punpcklwd xmm4, xmm1
            punpckhwd xmm5, xmm1
            cvtdq2ps xmm2, xmm2
            cvtdq2ps xmm3, xmm3
            cvtdq2ps xmm4, xmm4
            cvtdq2ps xmm5, xmm5
            mulps xmm2, xmm0
            mulps xmm3, xmm0
            mulps xmm4, xmm0
            mulps xmm5, xmm0
            haddps xmm2, xmm3
            haddps xmm4, xmm5
            haddps xmm2, xmm4
            cvtps2dq xmm2, xmm2 
            pshufb xmm2, xmm6
            movdqa [rsi + r11], xmm2
            add r11, 16
            cmp r11, r10
            jne .loop
        ;process remaining pixels
        sub r11, 16 ;Para procesar la última tira de 16 bytes, resto 16 bytes y sumo lo que me falta procesar       
        add r11, rdx
        movdqu xmm2, [rdi + r11]
        movdqa xmm4, xmm2
        punpcklbw xmm2, xmm1
        punpckhbw xmm4, xmm1
        movdqa xmm3, xmm2
        movdqa xmm5, xmm4
        punpcklwd xmm2, xmm1
        punpckhwd xmm3, xmm1
        punpcklwd xmm4, xmm1
        punpckhwd xmm5, xmm1
        cvtdq2ps xmm2, xmm2
        cvtdq2ps xmm3, xmm3
        cvtdq2ps xmm4, xmm4
        cvtdq2ps xmm5, xmm5
        mulps xmm2, xmm0
        mulps xmm3, xmm0
        mulps xmm4, xmm0
        mulps xmm5, xmm0
        haddps xmm2, xmm3
        haddps xmm4, xmm5
        haddps xmm2, xmm4
        cvtps2dq xmm2, xmm2 
        pshufb xmm2, xmm6
        movdqu [rsi + r11], xmm2
        
        xor r11, r11
        add rsi, r9
        add rdi, r8
	dec rcx
	cmp rcx, 0
    jne .loop
	ret
