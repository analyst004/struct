.CODE

; win_xchg (PULONG32 ecx (rcx), ULONG32 edx (rdx));
win_xchg PROC
    mov     rax, rdx
    xchg    [rcx], rax
	ret
win_xchg ENDP

;win_cmpxchg PROC StdCall Destination, Exchange, Comperand;
win_cmpxchg PROC 
    cmpxchg [rcx], rdx
    ret
win_cmpxchg ENDP

END