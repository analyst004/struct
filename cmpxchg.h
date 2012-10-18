#ifndef ASM_X86_CMPXCHG_H
#define ASM_X86_CMPXCHG_H

//#include <asm/alternative.h> /* Provides LOCK_PREFIX */

/*
 * Constants for operation sizes. On 32-bit, the 64-bit size it set to
 * -1 because sizeof will never return -1, thereby making those switch
 * case statements guaranteeed dead code which the compiler will
 * eliminate, and allowing the "missing symbol in the default case" to
 * indicate a usage error.
 */
#define __X86_CASE_B	1
#define __X86_CASE_W	2
#define __X86_CASE_L	4
#ifdef CONFIG_64BIT
#define __X86_CASE_Q	8
#else
#define	__X86_CASE_Q	-1		/* sizeof will never return -1 */
#endif



/* 
 * An exchange-type operation, which takes a value and a pointer, and
 * returns a the old value.
 */
#if defined(__GNUC__)
#define __xchg_op(ptr, arg, op, lock)					\
	({								\
	    __typeof__(*(ptr)) __ret = (arg);			\
		switch (sizeof(*(ptr))) {				\
		case __X86_CASE_B:					\
			asm volatile (#op "b %b0, %1\n"		\
				      : "+q" (__ret), "+m" (*(ptr))	\
				      : : "memory", "cc");		\
			break;						\
		case __X86_CASE_W:					\
			asm volatile (#op "w %w0, %1\n"		\
				      : "+r" (__ret), "+m" (*(ptr))	\
				      : : "memory", "cc");		\
			break;						\
		case __X86_CASE_L:					\
			asm volatile (#op "l %0, %1\n"		\
				      : "+r" (__ret), "+m" (*(ptr))	\
				      : : "memory", "cc");		\
			break;						\
		case __X86_CASE_Q:					\
			asm volatile (#op "q %q0, %1\n"		\
				      : "+r" (__ret), "+m" (*(ptr))	\
				      : : "memory", "cc");		\
			break;						\
		default:						\
			asm volatile("int 3");				\
			break;\
		}							\
		__ret;							\
	})
#else
static inline
uint32_t
__fastcall
win_xchg(
    uint32_t volatile *Target,
    uint32_t Value
    )
{
//#if defined(_MSC_VER)
    __asm {
        mov     eax, Value
        mov     ecx, Target
        xchg    [ecx], eax
    }
 /*
#else
    __asm__
    (
    	"mov _Value, %eax \n"
    	"mov $Target, %ecx \n"
    	"xchg  %eax, (%ecx)\n"
   	);
#endif
*/
}

#define __xchg_op(ptr, arg, op, lock)	\
	win_xchg((volatile uint32_t*)ptr, arg)
#endif


/*
 * Note: no "lock" prefix even on SMP: xchg always implies lock anyway.
 * Since this is generally used to protect other memory information, we
 * use "asm volatile" and "memory" clobbers to prevent gcc from moving
 * information around.
 */
#define xchg(ptr, v)	__xchg_op((ptr), (v), xchg, "")

/*
 * Atomic compare and exchange.  Compare OLD with MEM, if identical,
 * store NEW in MEM.  Return the initial value in MEM.  Success is
 * indicated by comparing RETURN with OLD.
 */

#if defined(__GNUC__)
#define __raw_cmpxchg(ptr, old, new, size, lock)			\
({									\
	__typeof__(*(ptr)) __ret;					\
	__typeof__(*(ptr)) __old = (old);				\
	__typeof__(*(ptr)) __new = (new);				\
	switch (size) {							\
	case __X86_CASE_B:						\
	{								\
		volatile uint8_t *__ptr = (volatile uint8_t *)(ptr);		\
		__asm__ volatile(lock "cmpxchgb %2,%1"			\
			     : "=a" (__ret), "+m" (*__ptr)		\
			     : "q" (__new), "0" (__old)			\
			     : "memory");				\
		break;							\
	}								\
	case __X86_CASE_W:						\
	{								\
		volatile uint16_t *__ptr = (volatile uint16_t *)(ptr);		\
		asm volatile(lock "cmpxchgw %2,%1"			\
			     : "=a" (__ret), "+m" (*__ptr)		\
			     : "r" (__new), "0" (__old)			\
			     : "memory");				\
		break;							\
	}								\
	case __X86_CASE_L:						\
	{								\
		volatile uint32_t *__ptr = (volatile uint32_t *)(ptr);		\
		asm volatile(lock "cmpxchgl %2,%1"			\
			     : "=a" (__ret), "+m" (*__ptr)		\
			     : "r" (__new), "0" (__old)			\
			     : "memory");				\
		break;							\
	}								\
	case __X86_CASE_Q:						\
	{								\
		volatile uint64_t *__ptr = (volatile uint64_t *)(ptr);		\
		asm volatile(lock "cmpxchgq %2,%1"			\
			     : "=a" (__ret), "+m" (*__ptr)		\
			     : "r" (__new), "0" (__old)			\
			     : "memory");				\
		break;							\
	}								\
	default:							\
		asm volatile("int 3");				\
		break;\
	}								\
	__ret;								\
})
#else 

static inline
uint32_t
__fastcall
win_cmpxchg(
    uint32_t volatile *Destination,
    uint32_t Exchange,
    uint32_t Comperand
    )
{
    __asm {
        mov     eax, Comperand
        mov     ecx, Destination
        mov     edx, Exchange
        cmpxchg [ecx], edx
    }
}


#define __raw_cmpxchg(ptr, old, _new, size, lock)			\
	win_cmpxchg((volatile uint32_t*)ptr, (uint32_t)_new, (uint32_t)old)
/*
{	\
	uint32_t __ret;	\
		__asm {									\
		    mov     eax, old						\
		    mov     ecx, ptr 					\
		    mov     edx, _new					\
		    cmpxchg [ecx], edx				   \
		}					\
	ret = __ret; \
}
*/
#endif

#define LOCK_PREFIX ""
#define __cmpxchg(ptr, old, _new, size)					\
	__raw_cmpxchg((ptr), (old), (_new), (size), LOCK_PREFIX)

#define cmpxchg(ptr, old, _new)						\
	__cmpxchg(ptr, old, _new, sizeof(*(ptr)))

#endif	/* ASM_X86_CMPXCHG_H */
