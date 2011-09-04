; void freichen_asm (unsigned char *src, unsigned char *dst, int h, int w, int row_size)

global freichen_asm

%define SQRT_2 1.4142135623730950488016887

section .data
    shufb_magic_mask: db 0, 2, 1, 3, 2, 4, 3, 5, 4, 6, 5, 7, 6, 8, 7, 9
    freichen_mask: db -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1
    sqrt2_vector: dd SQRT_2, SQRT_2, SQRT_2, SQRT_2
    abs_mask: db -1, -1, -1, 127, -1, -1, -1, 127, -1, -1, -1, 127, -1, -1, -1, 127

section .text
%define src [ebp + 8]
%define dst [ebp + 12]
%define h [ebp + 16]
%define w [ebp + 20]
%define src_row_size [ebp + 24]
%define x [ebp - 4]
%define y [ebp - 8]
%define pixels_remainding [ebp - 12]
%define pixels_per_iteration 6
%define mxcsr_value [ebp - 16]

freichen_asm:
    push ebp
    mov ebp, esp
    
    sub esp, 16 
    
    push ebx
    push edi
    push esi
    
    sub DWORD w, 2
    sub DWORD h, 2
    
    xor edx, edx
    mov eax, DWORD w
    mov ecx, pixels_per_iteration
    div ecx
    mov DWORD pixels_remainding, edx
    
    mov DWORD x, 0
    mov DWORD y, 0
    
    mov ecx, src
    mov ebx, dst
    
    ;seteo el mxcr para truncar en la transformacion de float a int
    stmxcsr mxcsr_value
    mov eax,mxcsr_value
    or ah, 60h
    mov mxcsr_value, eax
    ldmxcsr mxcsr_value
    
loop_y:
    mov DWORD x, 0
    
    loop_x:
        mov ecx, src
        
        mov eax, y
        mov edi, src_row_size
        mul edi
        add ecx, eax
        add ecx, DWORD x
            
            
        ;cargamos las filas y-1, y, y+1 en los registros
        mov ebx, ecx
     
        movdqu xmm0, [ebx]
        add ebx, DWORD src_row_size 
        movdqu xmm1, [ebx]
        add ebx, DWORD src_row_size 
        movdqu xmm2, [ebx]
        
        movdqu xmm7, [shufb_magic_mask]
        movdqu xmm6, [freichen_mask]
        
        ;Calculamos la derivada en x, como prewit, pero la fila y+1 se pasa 
        ; a float y se multiplica por raiz de 2.
        movdqu xmm3, xmm0
        movdqu xmm4, xmm1
        movdqu xmm5, xmm2
                
        pshufb xmm3, xmm7
        pshufb xmm4, xmm7
        pshufb xmm5, xmm7

        pmaddubsw xmm3, xmm6
        pmaddubsw xmm4, xmm6
        pmaddubsw xmm5, xmm6
        
        ;sumo las filas y e y+2
        paddsw xmm3, xmm5
        
        ;guardo el xmm4
        movdqu xmm6, xmm4
        movdqu xmm7, xmm4
        
        ;extiendo a dwords xmm4, en los registros xmm6 y xmm7
        pxor xmm5, xmm5
        pcmpgtw xmm5, xmm6
        
        punpcklwd xmm6, xmm5
        punpckhwd xmm7, xmm5
        
        ;convierto a float los registros xmm6 y xmm7
        cvtdq2ps xmm6, xmm6
        cvtdq2ps xmm7, xmm7

        ;multiplico por raiz de 2 xmm6 y xmm7
        movdqu xmm4, [sqrt2_vector]
        mulps xmm6, xmm4
        mulps xmm7, xmm4
        
        ;extiendo a dwords xmm3
        pxor xmm5, xmm5
        pcmpgtw xmm5, xmm3
        
        movdqu xmm4, xmm3
        
        
        punpcklwd xmm3, xmm5
        punpckhwd xmm4, xmm5
        
        ;convierto a float los registros xmm3 y xmm4
        cvtdq2ps xmm3, xmm3
        cvtdq2ps xmm4, xmm4        
        
        ;sumo los registros de punto flotante
        addps xmm6, xmm3 
        addps xmm7, xmm4
        
        ;en xmm6 y xmm7 quedan los resultados parciales, no pueden usarse de aca
        ; en adelante
        
        ;Calculamos la derivada en y.
        ;Convertimos xmm0 y xmm2 a words
        pxor xmm1, xmm1
        
        punpcklbw xmm0, xmm1
        punpcklbw xmm2, xmm1
        
        psubw xmm2, xmm0
        
        movdqu xmm0, xmm2
        movdqu xmm1, xmm2
        
        psrldq xmm1, 2
        psrldq xmm2, 4
        
        paddw xmm0, xmm2
        
        ;extiendo a dwords xmm0 (en xmm0 y xmm1) y xmm1 (en xmm2 y xmm3)
        pxor xmm4, xmm4
        pcmpgtw xmm4, xmm0
        
        movdqu xmm2, xmm1; guardo el xmm1 porque lo voy a pizar
        movdqu xmm1, xmm0
        
        punpcklwd xmm0, xmm4
        punpckhwd xmm1, xmm4
        
        movdqu xmm3, xmm2
        
        pxor xmm4, xmm4
        pcmpgtw xmm4, xmm2
        
        punpcklwd xmm2, xmm4
        punpckhwd xmm3, xmm4
        
        ;convierto a float los registros xmm6 y xmm7
        cvtdq2ps xmm0, xmm0
        cvtdq2ps xmm1, xmm1
        cvtdq2ps xmm2, xmm2
        cvtdq2ps xmm3, xmm3
        
        ;multiplico por raiz de 2 xmm2 y xmm3
        movdqu xmm4, [sqrt2_vector]
        mulps xmm2, xmm4
        mulps xmm3, xmm4
        
        ;sumo los registros
        addps xmm0, xmm2
        addps xmm1, xmm3
        
        ;saco el valor absoluto de las derivadas parciales
        movdqu xmm2, [abs_mask]
        andps xmm0, xmm2
        andps xmm1, xmm2
        andps xmm6, xmm2
        andps xmm7, xmm2
        
        ;sumo las derivadas parciales
        addps xmm0, xmm6
        addps xmm1, xmm7
        
        ;transformo los resultados a dwords enteros con signo
        cvtps2dq xmm0, xmm0
        cvtps2dq xmm1, xmm1
        
        ;transformo los dwors con signo a words con signo y los junto
        packssdw xmm0, xmm1
        
        ;transformo los words con singo a bytes sin signo saturando
        packuswb xmm0, xmm0
        
        mov ecx, dst
        
        mov eax, y
        mov edi, src_row_size
        mul edi
        add ecx, eax
        add ecx, DWORD x
        
        
        mov ebx, ecx
        add ebx, DWORD src_row_size
        inc ebx
        
        movd eax, xmm0
        mov [ebx], eax
        
        psrldq xmm0, 4
        movd eax, xmm0
        mov WORD [ebx + 4], ax

        add DWORD x, pixels_per_iteration
        mov edi, DWORD w
        cmp DWORD x, edi
        je end_loop_x
        
        sub edi, pixels_per_iteration
        cmp DWORD x, edi
        jg process_remainding_pixels
        jmp loop_x
        
    
    end_loop_x:
    
    add DWORD y, 1
    
    mov edi, DWORD y
    cmp edi, DWORD h
    jl loop_y
    xor eax, eax
    
    pop esi
    pop edi
    pop ebx

    add esp, 16
    pop ebp
    ret    
        
        
process_remainding_pixels:     
    mov edi, pixels_remainding
    sub DWORD x, pixels_per_iteration
    add DWORD x, edi
    
    jmp loop_x
