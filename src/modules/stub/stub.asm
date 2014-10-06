;; ---------------------------------------
;; asm_tick
;;
;; rdi: Uint8 buttons
;; rsi: ptr Uint8 pixels[w*h*4]
;; rdx: Uint32 elapsed_ms
;; return int 0 or 1 for quit
;; ---------------------------------------

        global asm_tick         ; main entrypoint
        section .text
asm_tick:

        test    rdi, rdi        ; test buttons for non-zero
        jnz quit
        
        mov     rax, 0          ; don't quit
        ret                     ; end tick
        
quit:
        mov     rax, 1          ; quit
        ret                     ; end tick
