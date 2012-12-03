.686p
.model flat 
option casemap:none
.CODE

win_xchg PROC StdCall Target, Value
    mov     rax, Value
    mov     rcx, Target
    xchg    [rcx], rax
	ret
win_xchg ENDP

win_cmpxchg PROC StdCall Destination, Exchange, Comperand
    mov     rax, Comperand
    mov     rcx, Destination
    mov     rdx, Exchange
    cmpxchg [rcx], rdx
win_cmpxchg ENDP

END