#ifndef _JSON_H
#define _JSON_H
#if __GNUC__
#define _GNU_SOURCE
#endif
#include "../mvdef.h"
#define __NONNULL__ __attribute__((nonnull))
#define __UNUSED__ __attribute__((unused))
#define __FORCE_INLINE__ __attribute__((always_inline))
/* Do not change this for now. Non-buffering has'nt been handled */
#define CACHE_OBJSIZE 1

#ifdef JSON_MINIFY
#undef CACHE_OBJSIZE
#define NOBJ_METHODS
#define NOBJ_BUFFERING
#endif

size_t JsonGetSize(void *obj)
{ /*
    size_t sz;

    for (sz = 0; obj; sz++)
	obj->__nd;
    return sz;
    */
}

#ifdef JSON_DEBUG
#define throwError(errnum, mem, ...) __jsonError__(errnum, #__LINE__, #object ##->##__FUNC__, mem)
#else
#define throwError(errnum, ...) (errno = errnum)
#endif
#define puti(d) printf("%ld\n", (long)(d))
enum {
      ESYNTAX = 256,
      EINIT = 257
};

/*________________________________________________
//                                                |
//                 RSWITCH (8 bits)               |
//                                                |
//  0b     11      1  1    1     1    1     1     |
//     [OBJ, ARR] []  [] [KEY] [VAL]  [] [START]  |
//                                                |
//                                                |
//________________________________________________|
*/
#endif
