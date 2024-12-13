#include "json.h"
#include <stdbool.h>
#include <errno.h>

#if SIG_SPRT == 1
#include <signal.h>
#endif
#define PASS (void)0

#define OBJHASH_BUCKET 8
#define F_OBJEXIST 0x01
#define F_NWRITE 0x02
#define MIN_FILE_BYTES 7
#define MAX_FILE_BYTES SSIZE_MAX

#ifdef _WINDOW_H
#include <wchar.h>
#include <wctype.h>
#define isspace(c) iswspace(c)
#define Json_strLen(s) wcstrlen(s)
typedef wchar_t JsonChar;
typedef JsonChar * JsonString;
#define JS_CH(txt) L txt
#else
#include <ctype.h>
#define JS_CH(txt) txt
typedef uint8_t  JsonChar;
typedef JsonChar * JsonString;
#define Json_strLen(s) strlen(s)
#endif
typedef intmax_t JsonInteger;
typedef double JsonDecimal;
typedef void * JsonArray;
typedef void * JsonDict;
typedef _Bool JsonBool;

typedef struct JsonObjectStruct JsonObjectStruct;
typedef JsonObjectStruct * JsonObject;
typedef struct JsonDataStruct _JsonDataStruct;
typedef struct JsonDataStruct * JsonData;

/* PROTOTYPES */
__NONNULL__ static __inline__ void *lexer(JsonObject, JsonString, ssize_t);

struct _JsonDataStruct {
    JsonString *__key;
    union {
	JsonString __str;
	JsonArray __arr;
	JsonDict __dict;
	JsonDecimal __flt[1];
	JsonInteger __int[1];
	JsonBool __bool[1];
    } __data;
    JsonData __nd;
    JsonData __pd;
    uint8_t __type;
};
struct JsonObjectStruct {
    JsonData __head[OBJHASH_BUCKET];
    JsonData __tail[OBJHASH_BUCKET];
    JsonObject *__self;
    void * (*jsonOpen)(JsonObject *,  char *);
    void * (*jsonRead)(JsonObject *,  char *);
    void * (*jsonDelete)(JsonObject *,  char *);
    void * (*jsonDump)(JsonObject *,  char *);
    int __hashBkt;
    unsigned int __setfl;
};

#ifdef JSON_DEBUG
#define throwError(errnum, mem, ...) __jsonError__(errnum, #__LINE__, #object ##->##__FUNC__, mem)
#else
#define throwError(errnum, ...) (errno = errnum)
#endif

enum {
    ESYNTAX = 7
};

__NONNULL__ static void *__init__(JsonObject self, char *file) {
    register int L0 __UNUSED__;
    register int fd;
    struct stat statbf;

    if (*file == 0 || self && (self->__setfl & F_OBJEXIST)) {
	throwError(EINVAL, self);
	return NULL;
    }
    if ((fd = open(file, O_RDWR | O_NONBLOCK | O_NOFOLLOW))) {
	throwError(errno, self);
	return NULL;
    }
    if (fstat(fd, &statbf)) {
	throwError(errno, self);
	return NULL;
    }
    if (statbf.st_size < MIN_FILE_BYTES) {
    }
    if (! (self = malloc(sizeof (JsonObject)))) {
	throwError(ENOMEM, self);
	return NULL;
    }

    for (L0 = 0; L0 < OBJHASH_BUCKET; L0++) {
	self->__head[L0] = self->__tail[L0] = NULL;
    }
    self->__setfl |= F_OBJEXIST;
    self->__self = &self;

    return self;
}
static __inline__ void *readfl(JsonObject obj, int fd, ssize_t sz) {
    JsonString bf;
    register ssize_t rdbytes, L1;

    if (fd < 0 || sz < 0) {
	return 0;
    }
    bf = malloc(sz + 1);

    if (sz > MAX_FILE_BYTES) {
	/* TODO: read block by block ( each block reads MAX_FILE_BYTES bytes )  */
    }
    if (bf == NULL) {
	throwError(ENOMEM, NULL);
	return NULL;
    }

    while (((rdbytes = read(fd, bf, sz)) != -1) || (rdbytes == EINTR)) {
	/* TODO: update file pointer using seek if EINTR */
	continue;
    }
    if (rdbytes < sz) {
	/* incomplete read */
	/* TODO: read the complete bytes per a byte using getchar */
    }
    bf[rdbytes] = 0;

    return lexer(obj, bf, sz);
}

__NONNULL__ static __inline__ ssize_t getKey(JsonString str, JsonString __restrict *bf) {
    register JsonChar c;
    register ssize_t rsz, i;

    if (*str == 0)
	return -1;
    /* TODO: REMOVE THE STOP AT NULL BYTE */
    for (rsz = 0; (c = str[rsz]) && !(c == JS_CH('"')); rsz++)
	PASS;
    if (c == JS_CH('"')) {
	for (i = rsz + 1; (c = str[i]) && isspace(c); i++)
	    PASS;
	if (c == JS_CH(':')) {
	    *bf = malloc(rsz + 1);
	    if (*bf == NULL) {
		throwError(ENOMEM, NULL);
		return -1;
	    }
	    memcpy(*bf, str, rsz);
	    (*bf)[rsz] = 0;
	    return ++i;
	}
    }
    return -1;
}

#define EXP_VAL 0x04
#define EXP_KEY 0x08
#define MBLK_OBJ 0x80
#define MBLK_ARR 0x40
#define EXP_VAL_R_KEY 0x0c
#define NEXP_STR_ARR_OBJ 0x3f
#define MBLK_START 0x01
#define MBLK_CLOSE 0xe1
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
#define R_EXP_KEY EXP_VAL
#define R_EXP_VAL EXP_KEY
/* set accept key or value */
#define SET_OPFLAG(AT, FL) (void)((AT) = (((AT) & MBLK_CLOSE) | FL))
#define M0V_BUFFER_PTR(bf, at) ((bf) += (at))
#define EOVAL(c)							\
    (!(((c) ^ JS_CH(',')) && ((c) ^ JS_CH('\n')) && ((c) ^ JS_CH('}')) && ((c) ^ JS_CH(']'))))

#define puti(d) printf("%ld\n", (long)(d))
__NONNULL__ static __inline__ void *lexer(JsonObject obj, JsonString bf, ssize_t sz) {
    register ssize_t objMtch, arrMtch, nkeys, L2, L3, LINE, CH_AT;
    register JsonChar c;
    uint8_t rswitch;

    JsonString k;
    JsonString va;
    LINE = objMtch = arrMtch = rswitch = 0;
    for (L2 = 0; (c = bf[L2]); L2++) {
	if ((c == JS_CH('\n') ? LINE++ : 0) || isspace(c))
	    continue;
	switch (c) {
	case JS_CH('{'):
	    objMtch += 1;
	    if (rswitch & EXP_VAL) {
		putchar(c);
	    }
	    rswitch |= MBLK_OBJ | MBLK_START | EXP_KEY;
	    break;
	case JS_CH('}'):
	    /* TODO: handle trailing braces @anonymous position */
	    if ((rswitch & EXP_VAL_R_KEY))
		goto syntaxError;
	    if (objMtch > 1)
		puts("}");
	    rswitch &= MBLK_CLOSE;
	    objMtch -= 1;
	    break;
	case JS_CH(':'):
	    puts("trailing colon");
	    goto syntaxError;
	    break;
	case JS_CH('"'):
	    if (rswitch & EXP_KEY) {
		if ((L3 = getKey(bf + ++L2, &k)) == -1) {
		    goto syntaxError;
		}
		k[L3] = 0;
		M0V_BUFFER_PTR(bf, L2 + L3);
		L2 = -1;
		SET_OPFLAG(rswitch, EXP_VAL);
		printf("%s: ", k);
	    }
	    else {
		if (!(rswitch & MBLK_START)) {
		    goto syntaxError;
		}
		M0V_BUFFER_PTR(bf, ++L2);
		L2 = 0;
		for (; (c = bf[L2]) && (c != JS_CH('"')); L2++)
		    PASS;
		if (c == 0) {
		    goto syntaxError;
		}
		if ((va = malloc(L2 + 1)) == NULL) {
		    throwError(ENOMEM, NULL);
		    return NULL;
		}
		memcpy(va, bf, L2);
		M0V_BUFFER_PTR(bf, ++L2);
		va[L2] = 0;
		L2 = -1;
		rswitch &= MBLK_CLOSE;
		puts(va);
	    }
	    break;
	case JS_CH('['):
	    if (!(rswitch & MBLK_START)) {
		goto syntaxError;
	    }
	    arrMtch += 1;
	    rswitch |= MBLK_ARR;
	    SET_OPFLAG(rswitch, EXP_VAL);
	    break;
	case JS_CH(']'):
	    if (! --arrMtch) {
		/* close array */
		rswitch &= ~MBLK_ARR;
	    }
	    rswitch &= MBLK_CLOSE;
	    break;
	case JS_CH(','):
	    if (rswitch & EXP_VAL_R_KEY) {
		goto syntaxError;
	    }
	    rswitch & MBLK_ARR ? SET_OPFLAG(rswitch, EXP_VAL) : SET_OPFLAG(rswitch, EXP_KEY);
	    break;
	default:
	    if (!(rswitch & MBLK_START) || (rswitch & EXP_KEY)) {
	    syntaxError:
		throwError(ESYNTAX, NULL, LINE, L2);
		return NULL;
	    }
	    /* TODO: cut out preceeding space */
	    for (L3 = 0; (c = bf[L3]) && !EOVAL(c); L3++)
		putchar(c);
	    bf += L3;
	    L2 = -1;
	    rswitch &= MBLK_CLOSE;
	    puts("");
	}
    }
    printf("%ld\n", LINE);
}

void static __inline__ __jsonError__(int errn, char * __restrict line, char * __restrict objf, char *obj) {
}

/* HASHES */
#ifdef __UINT64_T__
#define HASH_int uint64_t
#define HASH_c1
#define HASH_c2
#define HASH_c3
#else
#define HASH_int uint32_t
#define HASH_c1
#define HASH_c2
#define HASH_c3
#endif
__NONNULL__ HASH_int getHashKeyFromByte (void *byte, ssize_t len, uint32_t seed) {
    HASH_int c1;

    c1 = 57722121L;
    while (len--) {
	c1 ^= *(char *)byte++ * 23423491L;
	c1 = (c1 * (seed << 13)) ^ 123;
	c1 *= 234239473L;
    }
    return (c1 ^ 9697619231L) ^ (c1 << 13), c1 ^= 11;
}
/*
  struct JsonData {
  JsonString *__key;
  union {
  JsonString __str;
  JsonArray __arr;
  JsonDict __dict;
  JsonDecimal __flt[1];
  JsonInteger __int[1];
  JsonBool __bool[1];
  } __data;
  JsonData *__nd;
  JsonData *__pd;
  int8_t __type;
  };
*/
__NONNULL__ JsonObject addStringToHashTable (JsonObject self, JsonString key, void *value, uint8_t type) {
    void *_g;
    size_t putAtTableIndex;
    JsonData newDAta;

    if (*key == 0)
	return self;
    putAtTableIndex = getHashKeyFromByte(key, Json_strLen(key), 123) % self->__hashBkt;
    newData = malloc(sizeof(newData));

    if (newData == NULL) {
	errno = ENOMEM;
	return jsonObject;
    }
    newData->__str = (JsonString)value;
    newData->__type = JSONSTRING;
}
int main(int argc, char **argv) {
    int fd;
    char bf[1025];
    ssize_t nr;

    if (argc != 2) {
	return -1;
    }

    if ((fd = open(argv[argc - 1], O_RDONLY)) == -1) {
	fprintf(stderr, "open: %s\n", strerror(errno));
	return -1;
    }
    if ((nr = read(fd, bf, 1024)) == -1) {
	fprintf(stderr, "%s\n", strerror(errno));
	return -1;
    }
    bf[nr] = 0;
    puti(getHashKeyFromByte("solpe", 5, 33) % 11);
    /*	if (lexer((JsonObject)1, bf, nr) == NULL) {
	puts("error");
	}*/
    return 0;
}

