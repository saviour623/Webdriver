#ifndef _JSON_H
#define _JSON_H
#if __GNUC__
#define _GNU_SOURCE
#endif
#include "../mvdef.h"
#define __NONNULL__ __attribute__((nonnull))
#define __UNUSED__ __attribute__((unused))
/* Do not change this for now. Non-buffering has'nt been handled */
#define JSON_DOBUFFERING 1

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
