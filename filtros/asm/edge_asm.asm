section .data
DEFAULT REL

cerocomacinco: dd 0.5, 0.5, 0.5, 0.5

section .text
global edge_asm
;void edge_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) 
;src rdi
;dst rsi
;h rdx
;w rcx
;src_rs r8
;dst_rs r9

%define src r15
%define dst r14
%define h r13
%define w r12
%define src_rs r11
%define dst_rs r10
%define dire rbx
%define remainding rsi

edge_asm:
    push rbp
    mov rbp, rsp
    
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push rbx

    mov src, rdi
    mov dst, rsi
    mov w, rcx
    mov h, rdx
    mov src_rs, r8
    mov dst_rs, r9
    
    pxor xmm15, xmm15
    movdqu xmm10, [cerocomacinco]

    xor rcx, rcx
    sub w, 2

    mov rax, w
    xor rdx, rdx
    mov rcx, 14
    idiv rcx 
    mov rcx, rax
    imul rcx, 14
    ;posiciono el puntero del destino en la posición inicial donde voy a empezar a copiar
    add dst, dst_rs
    add dst, 1

    sub h, 1
    mov w, rcx
    xor rcx, rcx

    .loop_filas:
        
        xor rax, rax
        .loop_columnas:


		;Cargamos las 3 filas necesarias para procesar los píxeles en xmm0, 1 y 2 respectivamente
		movdqu xmm0, [src + rax]
		add src, src_rs
		movdqu xmm1, [src + rax]
		add src, src_rs
		movdqu xmm2, [src + rax]
		sub src, src_rs
		sub src, src_rs
		;Copiamos xmm0
		movdqa xmm3, xmm0
		movdqa xmm4, xmm0
		movdqa xmm5, xmm0

		;Shifteamos xmm4 uno a la derecha
		psrldq xmm4, 1
		;Shifteamos xmm5 dos a la derecha
		psrldq xmm5, 2

		;Expandimos xmm3, la parte alta en xmm14, la parte baja en xmm13
		movdqa xmm13, xmm3
		movdqa xmm14, xmm3

		punpckhbw xmm14, xmm15
		punpcklbw xmm13, xmm15

		;Expandimos xmm5, la parte alta en xmm12, la parte baja en xmm11
		movdqa xmm12, xmm5
		movdqa xmm11, xmm5

		punpckhbw xmm12, xmm15
		punpcklbw xmm11, xmm15
		;Sumamos ambos registros (partes altas con partes altas, partes bajas con bajas)
		;quedandonos con la suma en xmm14 y xmm13.
		paddusw xmm14, xmm12
		paddusw xmm13, xmm11

		; Copiamos xmm2
		movdqa xmm6, xmm2
		movdqa xmm7, xmm2
		movdqa xmm8, xmm2

		; Shifteamos xmm7 uno a la derecha
		psrldq xmm7, 1
		; Shifteamos xmm8 dos a la derecha
		psrldq xmm8, 2
		; Expandimos xmm8, la parte alta a xmm12 y la baja a xmm11
		movdqa xmm12, xmm8
		movdqa xmm11, xmm8

		punpckhbw xmm12, xmm15
		punpcklbw xmm11, xmm15 
		; Sumamos estos registros a los que teníamos anteriormente
		paddusw xmm14, xmm12
		paddusw xmm13, xmm11

		; Expandimos xmm6, la parte alta a xmm12 y la baja a xmm11
		movdqa xmm12, xmm6
		movdqa xmm11, xmm6

		punpckhbw xmm12, xmm15
		punpcklbw xmm11, xmm15
		; Sumamos estos registros a los que teníamos anteriormente
		paddusw xmm14, xmm12
		paddusw xmm13, xmm11
		
		; Ordenamos un poco, movemos las columnas del medio (todavía sin expandir) a xmm3 y xmm4
		movdqa xmm3, xmm4
		movdqa xmm4, xmm7

		; Movemos xmm13 xmm14 a xmm5 y xmm7 y los expandimos a xmm6 y 8, respectivamente
		movdqa xmm5, xmm13
		movdqa xmm6, xmm13
		
		movdqa xmm7, xmm14
		movdqa xmm8, xmm14
		
		punpcklwd xmm5, xmm15
		punpckhwd xmm6, xmm15
		punpcklwd xmm7, xmm15
		punpckhwd xmm8, xmm15
		; La convertimos en punto flotante
		cvtdq2ps xmm5, xmm5
		cvtdq2ps xmm6, xmm6
		cvtdq2ps xmm7, xmm7
		cvtdq2ps xmm8, xmm8
		; Multiplicamos por 0.5
		mulps xmm5, xmm10
		mulps xmm6, xmm10
		mulps xmm7, xmm10
		mulps xmm8, xmm10
		; Convertimos a Double Word int
		cvtps2dq xmm5, xmm5
		cvtps2dq xmm6, xmm6
		cvtps2dq xmm7, xmm7
		cvtps2dq xmm8, xmm8
		; Empaquetamos a Word
		packusdw xmm5, xmm6
		packusdw xmm7, xmm8
		movdqa xmm6, xmm7
		; Expandimos las shifteadas una vez de byte a word
		movdqa xmm7, xmm3
		movdqa xmm8, xmm3

		punpcklbw xmm7, xmm15
		punpckhbw xmm8, xmm15

		paddusw xmm5, xmm7
		paddusw xmm6, xmm8

		movdqa xmm7, xmm4
		movdqa xmm8, xmm4

		punpcklbw xmm7, xmm15
		punpckhbw xmm8, xmm15

		paddusw xmm5, xmm7
		paddusw xmm6, xmm8
		; Nos reordenamos un poco, movemos todo esto a xmm3 y xmm4
		movdqa xmm3, xmm5
		movdqa xmm4, xmm6
		; Hacemos los mismos shifts y copias para la fila del medio
		movdqa xmm5, xmm1
		movdqa xmm6, xmm1
		movdqa xmm7, xmm1

		;Shifteamos xmm4 uno a la derecha
		psrldq xmm6, 1
		;Shifteamos xmm5 dos a la derecha
		psrldq xmm7, 2

		; Al del medio lo multiplicamos por -6
		movdqa xmm12, xmm6
		movdqa xmm11, xmm6
		pxor xmm13, xmm13
		pxor xmm14, xmm14

		punpckhbw xmm12, xmm15
		punpcklbw xmm11, xmm15

		psubsw xmm14, xmm12
		psubsw xmm13, xmm11
		psubsw xmm14, xmm12
		psubsw xmm13, xmm11
		psubsw xmm14, xmm12
		psubsw xmm13, xmm11
		psubsw xmm14, xmm12
		psubsw xmm13, xmm11
		psubsw xmm14, xmm12
		psubsw xmm13, xmm11
		psubsw xmm14, xmm12
		psubsw xmm13, xmm11

		;Expandimos lso registros y los sumamos
		movdqa xmm12, xmm5
		movdqa xmm11, xmm5

		punpckhbw xmm12, xmm15
		punpcklbw xmm11, xmm15

		paddsw xmm14, xmm12
		paddsw xmm13, xmm11

		movdqa xmm12, xmm7
		movdqa xmm11, xmm7

		punpckhbw xmm12, xmm15
		punpcklbw xmm11, xmm15
		;Sumamos ambos registros (partes altas con partes altas, partes bajas con bajas)
		;quedandonos con la suma en xmm14 y xmm13.
		paddsw xmm14, xmm12
		paddsw xmm13, xmm11
		; Las sumamos con lo que quedó
		paddsw xmm3, xmm13
		paddsw xmm4, xmm14
		; Empaquetamos a byte saturado
		packuswb xmm3, xmm4
		; Copiamos el resultado.
		movq [dst + rax], xmm3
		psrldq xmm3, 6
		add rax, 6
		movq[dst + rax], xmm3

		add rax, 8
		cmp rax, w
		jl .loop_columnas
        ;TODO: process remaining pixels.        
	sub rax, 14
	add rax, rdx
	;Cargamos las 3 filas necesarias para procesar los píxeles en xmm0, 1 y 2 respectivamente
	movdqu xmm0, [src + rax]
	add src, src_rs
	movdqu xmm1, [src + rax]
	add src, src_rs
	movdqu xmm2, [src + rax]
	sub src, src_rs
	sub src, src_rs
	;Copiamos xmm0
	movdqa xmm3, xmm0
	movdqa xmm4, xmm0
	movdqa xmm5, xmm0

	;Shifteamos xmm4 uno a la derecha
	psrldq xmm4, 1
	;Shifteamos xmm5 dos a la derecha
	psrldq xmm5, 2

	;Expandimos xmm3, la parte alta en xmm14, la parte baja en xmm13
	movdqa xmm13, xmm3
	movdqa xmm14, xmm3

	punpckhbw xmm14, xmm15
	punpcklbw xmm13, xmm15

	;Expandimos xmm5, la parte alta en xmm12, la parte baja en xmm11
	movdqa xmm12, xmm5
	movdqa xmm11, xmm5

	punpckhbw xmm12, xmm15
	punpcklbw xmm11, xmm15
	;Sumamos ambos registros (partes altas con partes altas, partes bajas con bajas)
	;quedandonos con la suma en xmm14 y xmm13.
	paddusw xmm14, xmm12
	paddusw xmm13, xmm11

	; Copiamos xmm2
	movdqa xmm6, xmm2
	movdqa xmm7, xmm2
	movdqa xmm8, xmm2

	; Shifteamos xmm7 uno a la derecha
	psrldq xmm7, 1
	; Shifteamos xmm8 dos a la derecha
	psrldq xmm8, 2
	; Expandimos xmm8, la parte alta a xmm12 y la baja a xmm11
	movdqa xmm12, xmm8
	movdqa xmm11, xmm8

	punpckhbw xmm12, xmm15
	punpcklbw xmm11, xmm15 
	; Sumamos estos registros a los que teníamos anteriormente
	paddusw xmm14, xmm12
	paddusw xmm13, xmm11

	; Expandimos xmm6, la parte alta a xmm12 y la baja a xmm11
	movdqa xmm12, xmm6
	movdqa xmm11, xmm6

	punpckhbw xmm12, xmm15
	punpcklbw xmm11, xmm15
	; Sumamos estos registros a los que teníamos anteriormente
	paddusw xmm14, xmm12
	paddusw xmm13, xmm11
	
	; Ordenamos un poco, movemos las columnas del medio (todavía sin expandir) a xmm3 y xmm4
	movdqa xmm3, xmm4
	movdqa xmm4, xmm7

	; Movemos xmm13 xmm14 a xmm5 y xmm7 y los expandimos a xmm6 y 8, respectivamente
	movdqa xmm5, xmm13
	movdqa xmm6, xmm13
	
	movdqa xmm7, xmm14
	movdqa xmm8, xmm14
	
	punpcklwd xmm5, xmm15
	punpckhwd xmm6, xmm15
	punpcklwd xmm7, xmm15
	punpckhwd xmm8, xmm15
	; La convertimos en punto flotante
	cvtdq2ps xmm5, xmm5
	cvtdq2ps xmm6, xmm6
	cvtdq2ps xmm7, xmm7
	cvtdq2ps xmm8, xmm8
	; Multiplicamos por 0.5
	mulps xmm5, xmm10
	mulps xmm6, xmm10
	mulps xmm7, xmm10
	mulps xmm8, xmm10
	; Convertimos a Double Word int
	cvtps2dq xmm5, xmm5
	cvtps2dq xmm6, xmm6
	cvtps2dq xmm7, xmm7
	cvtps2dq xmm8, xmm8
	; Empaquetamos a Word
	packusdw xmm5, xmm6
	packusdw xmm7, xmm8
	movdqa xmm6, xmm7
	; Expandimos las shifteadas una vez de byte a word
	movdqa xmm7, xmm3
	movdqa xmm8, xmm3

	punpcklbw xmm7, xmm15
	punpckhbw xmm8, xmm15

	paddusw xmm5, xmm7
	paddusw xmm6, xmm8

	movdqa xmm7, xmm4
	movdqa xmm8, xmm4

	punpcklbw xmm7, xmm15
	punpckhbw xmm8, xmm15

	paddusw xmm5, xmm7
	paddusw xmm6, xmm8
	; Nos reordenamos un poco, movemos todo esto a xmm3 y xmm4
	movdqa xmm3, xmm5
	movdqa xmm4, xmm6
	; Hacemos los mismos shifts y copias para la fila del medio
	movdqa xmm5, xmm1
	movdqa xmm6, xmm1
	movdqa xmm7, xmm1

	;Shifteamos xmm4 uno a la derecha
	psrldq xmm6, 1
	;Shifteamos xmm5 dos a la derecha
	psrldq xmm7, 2

	; Al del medio lo multiplicamos por -6
	movdqa xmm12, xmm6
	movdqa xmm11, xmm6
	pxor xmm13, xmm13
	pxor xmm14, xmm14

	punpckhbw xmm12, xmm15
	punpcklbw xmm11, xmm15

	psubsw xmm14, xmm12
	psubsw xmm13, xmm11
	psubsw xmm14, xmm12
	psubsw xmm13, xmm11
	psubsw xmm14, xmm12
	psubsw xmm13, xmm11
	psubsw xmm14, xmm12
	psubsw xmm13, xmm11
	psubsw xmm14, xmm12
	psubsw xmm13, xmm11
	psubsw xmm14, xmm12
	psubsw xmm13, xmm11

	;Expandimos lso registros y los sumamos
	movdqa xmm12, xmm5
	movdqa xmm11, xmm5

	punpckhbw xmm12, xmm15
	punpcklbw xmm11, xmm15

	paddsw xmm14, xmm12
	paddsw xmm13, xmm11

	movdqa xmm12, xmm7
	movdqa xmm11, xmm7

	punpckhbw xmm12, xmm15
	punpcklbw xmm11, xmm15
	;Sumamos ambos registros (partes altas con partes altas, partes bajas con bajas)
	;quedandonos con la suma en xmm14 y xmm13.
	paddsw xmm14, xmm12
	paddsw xmm13, xmm11
	; Las sumamos con lo que quedó
	paddsw xmm3, xmm13
	paddsw xmm4, xmm14
	; Empaquetamos a byte saturado
	packuswb xmm3, xmm4
	; Copiamos el resultado.
	movq [dst + rax], xmm3
	psrldq xmm3, 6
	add rax, 6
	movq[dst + rax], xmm3

    add src, src_rs
    add dst, dst_rs

    add rcx, 1
    cmp rcx, h
    jne .loop_filas



    pop rbx
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    pop rbp
	ret
