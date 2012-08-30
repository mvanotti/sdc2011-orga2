section .data
DEFAULT REL

%define upper_mask 0x08

;void smalltiles asm(unsigned char *src, unsigned char *dst, int h, int w, int src row size, int dst row size)
;                                  rdi                 rsi      rdx     rcx            r8          r9
section .text
global smalltiles_asm
smalltiles_asm:
	push r12
    push r13

	and dl, 0xFE    	; enmascar para dejarlo par
	and cl, 0xFE
	mov r10, rdx
	shr r10, 1

    mov rax, rcx
	shl rax, 2
    mov r13, rax

    xor rdx, rdx
    mov r12, 16
    idiv r12
    imul rax, 16
    mov rcx, rax

    ; rax = (w * 4) / 16 * 16 (convertirlo a bytes y hacer division entera)
    ; rdx = (w * 4) mod 16
    ; r13 = (w * 4)

	; r10 = h/2

	mov rcx, r10 ; Usamos rcx para iterar por las filas. 

	xor r11, r11

	mov r12, r9   ; offset entre el primer y cuarto cuadrante (altura)
    imul r12, r10

    shr r13, 1 ; es el ancho en bytes de cada smalltile
.loop:
            movdqa xmm1, [rdi + r11]
            pshufd xmm0, xmm1, upper_mask
            
            shr r11, 1
            
            movq [rsi + r11], xmm0 ;segundo cuadrante
            add r11, r13
            movq [rsi + r11], xmm0 ;primer cuadrante
            add r11, r12
            movq [rsi + r11], xmm0 ; cuarto cuadrante
            sub r11, r13
            movq [rsi + r11], xmm0 ; tercer cuadrante
            sub r11, r12
            
            shl r11, 1
            
            add r11, 16
            cmp r11, rax
            jne .loop
        ; process remaining pixels

        jmp .process_rmp
.return:

        xor r11, r11
        add rdi, r8
        add rdi, r8
        add rsi, r9
    loop .loop

    pop r13
	pop r12
	ret

.process_rmp:
    sub r11, 16
    add r11, rdx

    movdqu xmm1, [rdi + r11]
    pshufd xmm0, xmm1, upper_mask

    shr r11, 1

    movq [rsi + r11], xmm0 ;segundo cuadrante
    add r11, r13
    movq [rsi + r11], xmm0 ;primer cuadrante
    add r11, r12
    movq [rsi + r11], xmm0 ; cuarto cuadrante
    sub r11, r13
    movq [rsi + r11], xmm0 ; tercer cuadrante
    jmp .return


