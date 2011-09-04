; void prewitt_asm (unsigned char *src, unsigned char *dst, int h, int w, int row_size)

global prewitt_asm


section .data
    shufb_magic_mask: db 0, 2, 1, 3, 2, 4, 3, 5, 4, 6, 5, 7, 6, 8, 7, 9
    prewitt_mask: db -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1

section .text
%define src [ebp + 8]
%define dst [ebp + 12]
%define h [ebp + 16]
%define w [ebp + 20]
%define src_row_size [ebp + 24]
%define x [ebp - 4]
%define y [ebp - 8]
%define pixels_remainding [ebp - 12]
%define pixels_per_iteration 8
prewitt_asm:
    push ebp
    mov ebp, esp
    
    sub esp, 12    
    
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
        movdqu xmm6, [prewitt_mask]
        
        ;Calculamos la derivada en x
        movdqu xmm3, xmm0
        movdqu xmm4, xmm1
        movdqu xmm5, xmm2
                
        pshufb xmm3, xmm7
        pshufb xmm4, xmm7
        pshufb xmm5, xmm7

        pmaddubsw xmm3, xmm6
        pmaddubsw xmm4, xmm6
        pmaddubsw xmm5, xmm6
        
        paddsw xmm3, xmm4
        paddsw xmm3, xmm5
        
        movdqu xmm7, xmm3
        ;en xmm7 tenemos las derivadas en x de 8 pixeles en words
        
        ;Calculamos la derivada en y.
        ;Para el filtro vertical, podemos ignorar la fila xmm1 porque es 0.
        ;empaquetamos xmm0 y xmm2 para pasarlos a words
        pxor xmm1, xmm1 ;Ponemos nuestra mascara para los packs en xmm1
        
        movdqu xmm3, xmm0
        movdqu xmm4, xmm0
        psrldq xmm3, 1
        psrldq xmm4, 2
        
        movdqu xmm5, xmm2
        movdqu xmm6, xmm2
        psrldq xmm5, 1
        psrldq xmm6, 2
        
        punpcklbw xmm0, xmm1
        punpcklbw xmm2, xmm1
        punpcklbw xmm3, xmm1
        punpcklbw xmm4, xmm1                
        punpcklbw xmm5, xmm1                
        punpcklbw xmm6, xmm1
        
        paddusw xmm0, xmm3
        paddusw xmm0, xmm4
        
        paddusw xmm2, xmm5
        paddusw xmm2, xmm6
        
        
        psubsw xmm2, xmm0
        
        pabsw xmm1, xmm7
        pabsw xmm0, xmm2
        
        paddusw xmm0, xmm1
        
        ; Saturar xmm0
        pxor xmm1, xmm1
        packuswb xmm0, xmm1 
        
        
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
        mov DWORD [ebx + 4], eax

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

    add esp, 12    
    pop ebp
    ret    
        
        
process_remainding_pixels:     
    mov edi, pixels_remainding
    sub DWORD x, pixels_per_iteration
    add DWORD x, edi
    
    jmp loop_x
