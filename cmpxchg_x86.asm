.686p
.model flat 
option casemap:none
.CODE

win_xchg PROC StdCall Target, Value
    mov     eax, Value
    mov     ecx, Target
    xchg    [ecx], eax
	ret
win_xchg ENDP


win_cmpxchg PROC StdCall Destination, Exchange, Comperand
    mov     eax, Comperand
    mov     ecx, Destination
    mov     edx, Exchange
    cmpxchg [ecx], edx
    ret
win_cmpxchg ENDP

END