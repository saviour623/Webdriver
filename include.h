#ifndef MVPG_INCLUDE_H
#define MVPG_INCLUDE_H


/***********************************************************************

* C STANDARD/SYSTEM HEADER

***********************************************************************/

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdalign.h>
#include <errno.h>
#include <assert.h>


/***********************************************************************

* COMPILER SPECIFIC ATTRIBUTES/INTRINSICS

***********************************************************************/

#if defined(__GNUC__) || defined(__clang__)
    #define __FORCE_INLINE__ __attribute__((always_inline))
#elif defined(_MSC_VER) || defined(_WIN32) || defined(_win32)
    #define __FORCE_INLINE__ __forceinline
    #define TYPEOF(T) void
#else
    #define __FORCE_INLINE__
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define __MAY_ALIAS__ __attribute__((may_alias))
    #define __MB_UNUSED__ __attribute__((unused))
    #define __WARN_UNUSED__ __attribute__ ((warn_unused_result))
    #define __NONNULL__ __attribute__((nonnull))
    #define TYPEOF(T) __typeof__(T)
#else
    #define __MAY_ALIAS__
    #define __MB_UNUSED__
    #define __WARN_UNUSED__
    #define __NONNULL__
    #define TYPEOF(T) void
#endif

#define __STATIC_FORCE_INLINE_F static __inline__ __FORCE_INLINE__

/***********************************************************************

* TOOL MACROS

***********************************************************************/

/* Do nothing (python’s ’pass’) */
#define PASS (void)0


#define MACR_EMPTY_PARAM(...) 0, 0
#define MACR_INDIRECT_EVAL(a, ...) MACR_ACCEPT_FIRST(__VA_ARGS__)
#define MACR_ACCEPT_FIRST(a, ...) a
#define MACR_IGNORE_FIRST(...) MACR_INDIRECT_EVAL(__VA_ARGS__)
/* empty */
#define MACR_IF_EMPTY_0(a, ...) __VA_ARGS__
/* also empty */
#define MACR_IF_EMPTY_(a, ...) __VA_ARGS__
/* non-empty */
#define MACR_IF_EMPTY_1(a, ...) a
#define MACR_CAT(A, A1) MACR_INDIRECT_CAT(A, A1)
#define MACR_INDIRECT_CAT(A, A1) A ## A1

/* Assert if argument passed to macro is not empty */
#define MACR_NON_EMPTY(...) MACR_IGNORE_FIRST(MACR_EMPTY_PARAM  __VA_ARGS__(), 1)

/* Assert If argument to macro is empty, do true else or false (actions) */
#define MACR_DO_ELSE(_true, _false, ...) MACR_CAT(MACR_IF_EMPTY_, MACR_NON_EMPTY(__VA_ARGS__))(_true, _false)



/***********************************************************************

* MATH

***********************************************************************/

#define     MOD2(n, m) ((n) & ((m) - 1)) /* n % m (m is a power of 2) */
#define   MODP2(n, p2) MOD2(n, 1ULL << p2) /* N % 2^p2 */
#define PRVMULP2(n, m) ((n) - ((n) & ((m) - 1))) /* (multiple of 2^m) < n */
#define   NXTMUL(n, m) (((n) + ((m) - 1)) & ~((m) - 1)) /* {(multiple m) >= n (m is a power of 2)} */
#define NXTMULP2(n, m) ((((n) >> m) + 1) << m) /* {n < (multiple of 2^m) > n} */


/***********************************************************

 * SAFE INTEGER ARITHMETIC

************************************************************/

/* SAFE_MUL_ADD (__bMulOverflow,  __bAddOverflow, safeMulAdd) */
#if defined(__GNUC__) || defined(__clang__)
    #define __bMulOverflow(a, b, c) __builtin_mul_overflow(a, b, c)
    #define __bAddOverflow(a, b, c) __builtin_add_overflow(a, b, c)
#elif defined(_MSC_VER) || defined(_WIN32)
/* WINDOWS KENRNEL API FOR SAFE ARITHMETIC */
    #include <ntintsafe.h>#
    #define __bAddOverflow(a, b, c) (RtlLongAdd(a, b, c) == STATUS_INTEGER_OVERFLOW)
    #define __bMulOverflow(a, b, c) (RtlLongMul(a, b, c) == STATUS_INTEGER_OVERFLOW)
#else
    #define __bAddOverflow(a, b, c) !( ((a) < (ULONG_MAX - (b)))) && (*(c) = (a) + (b))
    #define __bMulOverflow(a, b, c) !( !(((a) >>(LONG_BIT>>1)) || ((b) >> (LONG_BIT>>1))) && ((*(c) = a * b), true)
#endif

/* Safe ADD and MUL */
__STATIC_FORCE_INLINE_F long safeUnsignedMulAdd(unsigned long a, unsigned long b, unsigned long c) {
  unsigned long res;

  assert(!__bMulOverflow(a, b, &res) && !__bAddOverflow(res, c, &res));
  return res;
}


/***********************************************************************

* DEBUG

***********************************************************************/
#define throwError(...) puts(__VA_ARGS__)
#define puti(i)         printf("%lld\n", (long long int)(i))

#endif
