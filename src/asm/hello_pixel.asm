;; ---------------------------------------
;; asm_tick
;;
;; rdi: Uint8 buttons
;; rsi: ptr Uint8 pixels[w*h*4]
;; rdx: Uint32 elapsed_ms
;; return int 0 or 1 for quit
;; ---------------------------------------

; parameters order:
; r9    ; 6th param
; r8    ; 5th param
; r10   ; 4th param
; rdx   ; 3rd param
; rsi   ; 2nd param
; rdi   ; 1st param

    global asm_tick         ; main entrypoint
    section .text
asm_tick:

    test rdi, rdi           ; test buttons for non-zero
    jnz quit


    
    ;; swap RSI/RDI so that rdi is pixel dst
    xor rsi, rdi
    xor rdi, rsi
    xor rsi, rdi

    ;; offset by one pixel row
    add rdi, 160*4

    ;; set rsi to a hardcoded value
    mov rsi, red
    call set_pixel
    
    add rdi, 1
    mov rsi, green
    call set_pixel

    add rdi, 1
    mov rsi, blue
    call set_pixel              
 
    mov     rax, 0          ; don't quit
    ret                     ; end tick
        
quit:
    mov     rax, 1          ; quit
    ret                     ; end tick

    ;; assumes rdi is pixel dst offset
    ;; rsi contains 32 bits of pixel data
set_pixel:
    mov ecx, 3
    rep movsb
    ret
    
    section .data
red:    db 0xff, 0x00, 0x00
green:  db 0x00, 0xff, 0x00
blue:   db 0x00, 0x00, 0xff
    
width:  db 160
height: db 90
