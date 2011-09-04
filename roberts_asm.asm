; void roberts_asm (unsigned char *src, unsigned char *src, int h, int w, int row_size)

global roberts_asm

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

roberts_asm:
    push ebp
    mov ebp, esp
    
    sub esp, 12    
    
    push ebx
    push edi
    push esi
    
    sub DWORD w, 1
    sub DWORD h, 1
    
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
            
            
        ;cargamos las filas y, y+1 en los registros
        mov ebx, ecx
     
        movdqu xmm0, [ebx]
        add ebx, DWORD src_row_size 
        movdqu xmm1, [ebx]
        
        ;Calculamos la derivada en x
        movdqu xmm2, xmm0
        movdqu xmm3, xmm1
        
        psrldq xmm3, 1
        
        pxor xmm4, xmm4
        
        punpcklbw xmm2, xmm4
        punpcklbw xmm3, xmm4
        
        psubsw xmm2, xmm3
        
        pabsw xmm5, xmm2
        
        ;Calculamos la derivada en y.
        movdqu xmm2, xmm0
        movdqu xmm3, xmm1
        
        psrldq xmm2, 1
        
        pxor xmm4, xmm4
        
        punpcklbw xmm2, xmm4
        punpcklbw xmm3, xmm4
        
        psubsw xmm2, xmm3
        
        pabsw xmm6, xmm2

        ;calculamos el valor final
        
        paddsw xmm5, xmm6
        
        ; Saturar xmm5
        pxor xmm1, xmm1
        packuswb xmm5, xmm1 
        
        movdqu xmm0, xmm5
        
        mov ecx, dst
        
        mov eax, y
        mov edi, src_row_size
        mul edi
        add ecx, eax
        add ecx, DWORD x
        
        mov ebx, ecx
        
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
