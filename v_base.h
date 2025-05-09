#ifndef V_BASE_H
#define V_BASE_H

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

#define throwError(...) puts(__VA_ARGS__ "\n")
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

  /**
 * MACR_NON_EMPTY,
 * MACR_DO_ELSE
 *
 * Safely assert if a macro parameter is given an argument
 * Return: provided argument(s)/zero/nothing
 */
#define MACR_EMPTY_PARAM(...) 0, 0
#define MACR_INDIRECT_EVAL(a, ...) MACR_ACCEPT_FIRST(__VA_ARGS__)
#define MACR_ACCEPT_FIRST(a, ...) a
#define MACR_IGNORE_FIRST(...) MACR_INDIRECT_EVAL(__VA_ARGS__)
#define MACR_NON_EMPTY(...) MACR_IGNORE_FIRST(MACR_EMPTY_PARAM  __VA_ARGS__(), 1)
# /* empty */
#define MACR_IF_EMPTY_0(a, ...) __VA_ARGS__
# /* also empty */
#define MACR_IF_EMPTY_(a, ...) __VA_ARGS__
# /* non-empty */
#define MACR_IF_EMPTY_1(a, ...) a
#
#define MACR_CAT(A, A1) MACR_INDIRECT_CAT(A, A1)
#define MACR_INDIRECT_CAT(A, A1) A ## A1
#define MACR_DO_ELSE(_true, _false, ...) MACR_CAT(MACR_IF_EMPTY_, MACR_NON_EMPTY(__VA_ARGS__))(_true, _false)
#
# /* end */
#

#endif
