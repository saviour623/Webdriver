//#ifdef V_BASE_H
//#undef V_BASE_H
//#define V_BASE_H

#if defined(__GNUC__) || defined(__clang__)
#define __FORCE_INLINE__ __attribute__((always_inline))
#elif defined(_MSC_VER) || defined(_WIN32) || defined(_win32)
#define __FORCE_INLINE__ __forceinline
#else
#define __FORCE_INLINE__
#endif
#define __NONNULL__ __attribute__((nonnull))
#define CHOOSE_EXPR(cExpr, tVal, fVal)			\
  __builtin_choose_expr(cExpr, tVal, fVal)

#define throwError(...) (void *)0
#define puti(i) printf("%lld\n", (long long int)(i))

#if defined(__GNUC__) || defined(__clang__)
#define __MAY_ALIAS__ __attribute__((may_alias))
#define __EXPR_LIKELY__(T, L) __builtin_expect(T, L)
#define __MB_UNUSED__ __attribute__((unused))
#else
#define __MAY_ALIAS__
#define __EXPR_LIKELY__(T, L) T
#define __MB_UNUSED__
#endif
#define __STATIC_FORCE_INLINE_F static __inline__ __FORCE_INLINE__

//#endif
