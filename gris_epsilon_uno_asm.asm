; void gris_epsilon_inf_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size)

global gris_epsilon_uno_asm

section .data
    shf_mask_mem2xmm: db 00h, 80h, 03h, 80h, 06h, 80h, 09h, 80h, 0Ch, 80h, 0Fh, 80h, 80h, 80h, 80h, 80h

%define src [ebp + 8]
%define dst [ebp + 12]
%define h [ebp + 16]
%define w [ebp + 20]
%define src_row_size [ebp + 24]
%define dst_row_size [ebp + 28]
%define x [ebp - 4]
%define y [ebp - 8]
%define pixels_remainding [ebp - 12]

section .text
gris_epsilon_uno_asm:
    
    push ebp
    mov ebp, esp
    
    sub esp, 12
    
    push ebx
    push edi
    push esi
    
    xor edx, edx
    mov eax, DWORD w
    mov ecx, 6
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
    
        mov eax, x
        mov edi, 3
        mul edi
        add ecx, eax
        
        mov ebx, dst
        
        mov eax, y
        mov edi, dst_row_size
        mul edi
        add ebx, eax
    
        add ebx, DWORD x
        
        movdqu xmm0, [ecx]
        movdqu xmm1, [ecx + 1]
        movdqu xmm2, xmm1
        movdqu xmm3, [ecx + 2]
        
        movdqu xmm4, [shf_mask_mem2xmm]
        
        pshufb xmm0, xmm4
        pshufb xmm1, xmm4
        pshufb xmm2, xmm4
        pshufb xmm3, xmm4

        paddusw xmm0, xmm1
        paddusw xmm0, xmm2
        paddusw xmm0, xmm3
        
        psraw   xmm0, 2
        packuswb xmm0, xmm3

        movd eax, xmm0
        mov DWORD [ebx], eax
        
        psrldq xmm0, 4
        movd eax, xmm0
        mov WORD [ebx + 4], ax

        add DWORD x, 6
        mov edi, DWORD w
        cmp DWORD x, edi 
        je end_loop_x
        
        sub edi, 6
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
    sub DWORD x, 6
    add DWORD x, edi
    
    jmp loop_x
