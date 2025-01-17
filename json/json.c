#include "json.h"
#include <stdbool.h>
#include <errno.h>

#if SIG_SPRT == 1
#include <signal.h>
#endif
#define PASS (void)0

#define OBJHASH_BUCKET 11
#define F_OBJEXIST 0x01
#define F_NWRITE 0x02
#define MIN_FILE_BYTES 7
#define MAX_FILE_BYTES SSIZE_MAX

#if defined(_WINDOW_H) 
#include <wchar.h>
#include <wctype.h>
#define isspace(c) iswspace(c)
#define strcmp(s1, s2) wcscmp(s1, s2)
#define Json_strLen(s) wcstrlen(s)
typedef wchar_t JsonChar;
#define JS_CH(txt) L txt
#else
#include <ctype.h>
#define JS_CH(txt) txt
typedef uint8_t  JsonChar;
#define Json_strLen(s) strlen(s)
#endif
typedef JsonChar * JsonString;
typedef intmax_t JsonInteger;
typedef double JsonDecimal;
typedef void ** JsonArray;
typedef void ** JsonObj;
typedef _Bool JsonBool;

enum {
      JSONSTRING_TYPE,
      JSONINTEGER_TYPE,
      JSONDECIMAL_TYPE,
      JSONARRAY_TYPE,
      JSONOBJECT_TYPE,
      JSONBOOL_TYPE
};

typedef struct JsonObjectStruct JsonObjectStruct;
typedef struct JsonObjectStruct * JsonObject;
typedef struct JsonDataStruct JsonDataStruct;
typedef struct JsonDataStruct * JsonData;

#define SIZEOF_JSON_OBJECT sizeof(struct JsonObjectStruct)
#define SIZEOF_JSON_DATA sizeof(struct JsonDataStruct)

/*data struct */
struct JsonDataStruct {
    JsonString __key;
    union {
	JsonString __str;
	JsonArray __arr;
	JsonObj __obj;
	JsonDecimal __flt[1];
	JsonInteger __int[1];
	JsonBool __bool[1];
    };
    JsonData __nd;
    JsonData __pd;
    uint8_t __type;
};
/* object struct */
struct JsonObjectStruct {
    JsonData __head[OBJHASH_BUCKET];
    JsonData __tail[OBJHASH_BUCKET];
#ifdef CACHE_OBJSIZE
    size_t __sz[OBJHASH_BUCKET];
#endif
    JsonObject *__self, *__parent;
#if !defined(NOBJ_METHODS)
    void * (*string)(JsonObject, JsonString, void *);
    void * (*read)(JsonObject, char *);
    void * (*remove)(JsonObject, JsonString);
    void * (*delete)(JsonObject, char *);
    void * (*jsonDump)(JsonObject *,  char *);
#endif
#if !defined(NOBJ_BUFFERING)
    void *__buf;
    ssize_t __szbuf;
#endif
    uint8_t __hashBkt, __setfl;
};
/* print format struct */
typedef struct {
    uint8_t indent_char;
    uint8_t indent_width;
    uint8_t sorted;
    uint8_t raw;
} printfmt;

/* obj stack */
typedef struct objStack {
    void *stk_obj;
    struct objStack *stk_mov;
} objStack;

/* PROTOTYPES */
#if !defined(CACHE_OBJSIZE)
extern __inline__ size_t jsonGetSize(void *obj);
#endif
__NONNULL__ static __inline__ void *lexer(JsonObject, JsonString, ssize_t);
__NONNULL__ void *addToJsonObject (JsonObject, JsonString, void *, uint8_t);
__NONNULL__ void *addStringToHashTable (JsonObject, JsonString, void *);
__NONNULL__ void *removeFromHashTable (JsonObject, JsonString);
static __inline__ void *readfl(JsonObject, int, ssize_t);
__NONNULL__ static ssize_t dump(JsonObject self, ...);
__NONNULL__ ssize_t dumpbf(JsonObject, printfmt *);

#ifdef CACHE_OBJSIZE
#define GET_SIZE(_obj, _at) ((_obj)->__sz[(_at)])
#else
#define GET_SIZE(_obj, _at) jsonGetSize(_obj)
#endif

#define STACK_init(...) ((objStack *)NULL)
static __inline__ __FORCE_INLINE__ objStack *popFromStack(objStack **top) {
    objStack *remTop;

    return top && *top ?
	(remTop = *top),
	(*top = (*top)->stk_mov), remTop : (objStack *)NULL;
}
static __inline__ void *pushToStack(objStack **top, void *obj) {
    objStack *newTop;

    if (! (newTop = malloc(sizeof(objStack)))) {
	return NULL;
    }
    newTop->stk_obj = obj;
    newTop->stk_mov = (top == NULL || *top == NULL) ? NULL : *top;
    *top = newTop;
}
static __inline__ __FORCE_INLINE__ void *getFromStack(objStack **top) {
    return (top && *top) ? (*top)->stk_obj : NULL;
}

/*
 * vector
 */
typedef void ** JVEC_t;
typedef uint32_t VEC_szType;
typedef uint8_t word8;

#define VEC_DATA_BLOCK_SZ (sizeof (void *))
#define VEC_META_DATA_SZ(_pre_sz) ((_pre_sz & 0x0f) + 1) /*vector_byte_size and 1 byte for other meta-info */
#define VEC_LEAST_SZ 1
#define VEC_ALWYS_PREALLOC 0x40
#define VEC_ALLOC_SZ 1
#define VEC_APPEND 0xde
#define VEC_VECTOR 0x80
#define VEC_ARRAY 0
#define VEC_SAFE_INDEX_CHECK 1
#define VEC_ALW_WARNING 1
#define VEC_EROUT_OF_BOUND EINVAL

/* remove the type alignment of bytes so that each block can be addressed like a byte array */
#define VEC_ACCESS(_addr) ((word8 *)(void *)(_addr))

#define VEC_SZ_INCR(vec, fl)			\
    switch ((fl) & 0x0f) {					\
    case 0x01: *(volatile word8 *)(vec) += 1; break;		\
    case 0x02: *(volatile uint16_t *)(vec) += 1; break;		\
    case 0x03: *(volatile uint32_t *)(vec) += 1; break;		\
    case 0x04: *(volatile uint64_t *)(vec) += 1; break;		\
    } (void)0

#define VEC_SZEOF(_SZ)				\
    ((_SZ) > UINT8_MAX ?			\
     (_SZ) > UINT16_MAX ?			\
     (_SZ) > UINT32_MAX ?			\
     (_SZ) > UINT64_MAX ?			\
     0x05 : 0x04 : 0x03 : 0x02 : 0x01		\
     )
#define VEC_BLOCK_START(vec, fl)\
    (VEC_ACCESS(vec) - (((fl) & 0x0f) + 1))

#define VEC_MOVTO_DATA_START(vec, fl)				\
    ((vec) = (void *)(VEC_ACCESS(vec) + VEC_META_DATA_SZ(fl)))

static __inline__ __FORCE_INLINE__ void *VEC_getSize(void *vec, void *to) {
    register word8 *s, *v, i;

    v = vec, s = to;

    i = *--v & 0x0f; /* n bytes of vec size */
    v -= i; /* mov v to block start */

    while ( i-- ) {
	*s++ = *v++;
    }
    return to;
}

/*
 * vector_create
 */
static void **VEC_create(size_t vecSize) {
    void *vec;
    word8 mSz;

    if ( !vecSize )
	vecSize = VEC_LEAST_SZ;
    /* meta-data size */
    mSz = VEC_META_DATA_SZ(VEC_SZEOF(vecSize));
 
    if (! (vec = malloc((VEC_DATA_BLOCK_SZ * vecSize) + mSz)))
	return NULL;
    /* initialize size to zero */
    memset(vec, 0, --mSz);

    /* update meta-data: prealloc | type | n bytes allocated for sz ([1100 0001] for size == 1) */
    VEC_ACCESS(vec)[mSz] = ((word8)(vecSize > VEC_LEAST_SZ) << 6) | VEC_VECTOR | mSz;
    /* mov ahead meta-data block (main) */
    VEC_MOVTO_DATA_START(vec, mSz);
/* initialize the first block (main) to 0 [vector is empty] */
    *(void **)vec = 0;

    return vec;
}

/*
 * vector_append: add to last; realloc/copy vector if neccessary
 */
static __inline__  __NONNULL__ void **VEC_append(void ***vec, void *new, VEC_szType sz, word8 meta) {
    void *v0;
    register word8 sb, pb;
    register uintmax_t memtb;

    sb = false; /* True if size reqires an additional block */
    pb = meta & 0x0f; /* n bytes of current size */;

    /* shifting pb bytes should result to 0 if (sz + 1) <= the maximum number pb bytes can represent */
    if ((sz + 1) >> (4ul << pb)) {
	(VEC_ACCESS(*vec) - 1)[0] = (meta & ~pb) | (pb + 1);
	sb = true;
    }
       /* resize vector || allocate an entire new block */
    memtb = (VEC_DATA_BLOCK_SZ * (sz + VEC_ALLOC_SZ)) + VEC_META_DATA_SZ(meta);

    v0 = *vec ? sb == false ? realloc(VEC_BLOCK_START(*vec, meta), memtb) : malloc(memtb + 1) : NULL;

    if (v0 == NULL)
	return (void **)NULL;

    if (sb) {
	/* We are here because we want to pad a new block for size */
	memcpy(v0, &sz, pb);
	(VEC_ACCESS(v0) + pb)[0] = meta;

	/* (local realloc)
	 * This is a trade-off of performance and may be changed in future review.
	 */ 
	memcpy(v0 + meta + 1, *vec, sz * VEC_DATA_BLOCK_SZ);
	free(VEC_BLOCK_START(*vec, meta));
    }
    /* ++sz */
    VEC_SZ_INCR(v0, meta);
    
    *vec = (void *)(VEC_ACCESS(v0) + pb + 1);
    (*vec)[sz] = new; /* implicit [sz - 1] */

    return *vec;
}

/*
 * vec_expand: add new data to vector
 */
static __inline__  __NONNULL__ void **VEC_expand(void ***vec, void *vd, size_t index, size_t vflag) {
    VEC_szType sz, fl;

    (sz = 0) || VEC_getSize(*vec, &sz);

    fl = (VEC_ACCESS(*vec) - 1)[0];

    if (! (*vec)[0] ) {
#if VEC_SAFE_INDEX_CHECK || VEC_ALW_WARNING
	if ((index != 0) || (vflag != VEC_APPEND)) {
	    /* throw out-of-bound error */
	    goto err_outOfBound;
	}
#endif
	/* ignore index || index is 0 */
	(*vec)[index] = vd;
	/* ++sz */
	VEC_SZ_INCR(VEC_BLOCK_START(*vec, fl), fl);
    }
    else if (index < sz) {
	(*vec)[index] = vd;
    }
    else if (! ((vflag & VEC_APPEND) && VEC_append(vec, vd, sz, fl))) {	
	/* out of bound // insufficient memory */
    err_outOfBound:
#if VEC_ALW_WARNING
	vflag & VEC_APPEND && throwError(VEC_EROUT_OF_BOUND, vec, vd, index + 1, sz);
#endif
	return NULL;
    }
    return *vec;
}

static  __NONNULL__ void *VEC_add(void ***vec, void *vd, size_t bytesz, size_t sz, size_t index, word8 type) {
    void *v0 /* ptr to memory block */, **v00 /* ptr to vector */;
    word8 meta;
    size_t nalloc;

    v00 = (void *)1; /* prevent unsed v00 folding to 0 */

    meta = type | VEC_SZEOF(sz);
    nalloc = bytesz * sz; /* sizeof(vd) * sz */

    if (*vec == NULL || nalloc < 1
	|| /* block alloc  */ !(v0 = malloc(nalloc + VEC_META_DATA_SZ(meta)))) {
	return NULL;
    }
    memcpy(v0, &sz, meta & 0x0f);
    /* */
    VEC_MOVTO_DATA_START(v0, meta);
    (VEC_ACCESS(v0) - 1)[0] = meta;
    /* copy data to memory including its meta-data */
    memcpy(v0, vd, nalloc);

    /* if the vec_vector type is specified, create a new vector */
    if ((type & VEC_VECTOR) && (v00 = VEC_create(VEC_LEAST_SZ))) {
	/* initialize its first member with the data */
	*v00 = v0;
	/* reuse v0 to retain a generic referencing for both non-vector or vector using a void ptr */
	v0 = v00;
    }
    return v00 && VEC_expand(vec, v0, index, VEC_APPEND) ? v0 : NULL;
}

/*
 * vector: get, getAt, getType, remove, delete
 */
static __NONNULL__ __inline__ __attribute__((always_inline, pure)) void *VEC_getVectorItem(void **vec, ssize_t index) {
    size_t sz;

    (sz = 0) || VEC_getSize(*vec, &sz);
    index = sz + ( index < 0 ? index : 0 );

    if (index > sz || index < 0) {
#if VEC_ALW_WARNING
	throwError(VEC_EROUT_OF_BOUND, *vec, sz, index);
#endif
	return NULL;
    }
    return vec[index];
}
static __NONNULL__ __inline__ __FORCE_INLINE__ void *VEC_getVectorItemAt(void ***vec, ssize_t index, ssize_t at) {
    void *itemAt;

    itemAt = VEC_getVectorItem(*vec, at);
    return itemAt ? VEC_getVectorItem(&itemAt, index) : NULL;
}
static __inline__ __FORCE_INLINE__  __NONNULL__ uint8_t VEC_getType(void *vec) {
    /* v & VEC_VECTOR == 0 (array) */
    return ( (VEC_ACCESS(vec) - 1)[0] & VEC_VECTOR );
}
static __NONNULL__ void *VEC_remove(void ***vec, ssize_t index) {
}
static __NONNULL__ void *VEC_delete(void ***vec, ssize_t index) {
}

/*
 * hash
 */
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
__NONNULL__ __attribute__((pure)) HASH_int getHashKeyFromByte (void *byte, ssize_t len, uint32_t seed) {
    HASH_int c1;

    c1 = 57722121L;
    while (len--) {
	c1 ^= *(char *)byte++ * 23423491L;
	c1 = (c1 * (seed << 13)) ^ 123;
	c1 *= 234239473L;
    }
    return (c1 ^ 9697619231L) ^ (c1 << 13), c1 ^= 11;
}

__NONNULL__ static __inline__ __FORCE_INLINE__ void *addStringToJsonObject (JsonObject self, JsonString key, void *value) {
    return addToJsonObject(self, key, value, JSONSTRING_TYPE);
}
#define jsonGetIndex(key, bkt) (size_t)(getHashKeyFromByte((key), Json_strLen((key)), 123) % (bkt))
__NONNULL__ void *addToJsonObject (JsonObject self, JsonString key, void *value, uint8_t type) {
    register size_t _atIndex;
    JsonData newData;

    if (*key == 0)
	return self;
    if (! (newData = malloc(SIZEOF_JSON_DATA))) {
	errno = ENOMEM;
	return self;
    }
    newData->__key = key;

    switch (type) {
    case JSONSTRING_TYPE:
	newData->__str = value;
	newData->__type = JSONSTRING_TYPE;
        break;
    case JSONINTEGER_TYPE:
	*(newData->__int) = *(JsonInteger *)value;
	newData->__type = JSONINTEGER_TYPE;
	break;
    case JSONDECIMAL_TYPE:
	*(newData->__flt) = *(JsonDecimal *)value;
	newData->__type = JSONDECIMAL_TYPE;
	break;
    case JSONBOOL_TYPE:
	*(newData->__bool) = *(JsonBool *)value;
	newData->__type = JSONBOOL_TYPE;
	break;
    case JSONARRAY_TYPE:
	newData->__arr = value;
	newData->__type = JSONARRAY_TYPE;
	break;
    case JSONOBJECT_TYPE:
	newData->__obj = value;
	newData->__type = JSONOBJECT_TYPE;
	break;
    default:
	free(newData);
	newData = NULL;
	throwError(EINVAL, NULL);
	goto _end;
    }
    _atIndex = jsonGetIndex(key, self->__hashBkt);

    if (self->__head[_atIndex] == NULL) {
	newData->__pd = newData->__nd = NULL;
	self->__head[_atIndex] = self->__tail[_atIndex] = newData;
    }
    else {
	newData->__pd = self->__tail[_atIndex];
	newData->__nd = NULL;
	self->__tail[_atIndex] = newData;
    }
    (self->__sz[_atIndex])++;
 _end:
    return self;
}
#define IGNORE_EXPR(expr) ((void)(expr))
__NONNULL__ void *removeFromJsonObject (JsonObject self, JsonString key) {
    register size_t _fromIndex;
    JsonData iterItems;

    if (*key == 0)
	return self;
    _fromIndex = jsonGetIndex(key, self->__hashBkt);

    if (self->__head[_fromIndex] == NULL)
	goto error;

    iterItems = self->__head[_fromIndex];
    while (iterItems != NULL) {
	if (! strcmp(key, iterItems->__key)) {
	    IGNORE_EXPR(iterItems->__pd ? iterItems->__pd->__nd = iterItems->__nd : 0);
	    IGNORE_EXPR(iterItems->__nd ? iterItems->__nd->__pd = iterItems->__pd : 0);
	    break;
	}
	iterItems = iterItems->__nd;
    }
    if (iterItems == NULL) {
    error:
	throwError(EINVAL, "%s does not exist", key);
	return NULL;
    }
    if (--(self->__sz[_fromIndex]) == 0)
	self->__head[_fromIndex] = self->__tail[_fromIndex] = NULL;
    return iterItems;
}
__NONNULL__ static __inline__ void *HashTableToListMap (JsonObject self) {
    register size_t tableSize, i;

    tableSize = self->__hashBkt - 1;

    for (i = 0; i < tableSize; i++) {
	self->__tail[i]->__nd = self->__tail[i + 1];
	self->__head[i + 1]->__pd = self->__tail[i];
    }
    return self->__head[0];
}
__NONNULL__ static __inline__ void *ListMapToHsashTable (JsonObject self) {
    register size_t tableSize, i;

    tableSize = self->__hashBkt - 1;

    for (i = 0; i < tableSize; i++) {
	self->__head[i]->__pd = self->__tail[i]->__nd = NULL;
    }
}
/*
 * __init__
 */
__NONNULL__ __attribute__((warn_unused_result)) void *JsonInit(JsonObject *self) {
    register int L0 __UNUSED__;

    if (! (*self = malloc(SIZEOF_JSON_OBJECT))) {
	throwError(ENOMEM, *self);
	return NULL;
    }
    
    for (L0 = 0; L0 < OBJHASH_BUCKET; L0++) {
	(*self)->__head[L0] = (*self)->__tail[L0] = NULL;
    }

    (*self)->__setfl |= F_OBJEXIST;
    (*self)->__hashBkt = OBJHASH_BUCKET;
    (*self)->__self = self;
    /* __parent will be modified during object attachment for non-base objects, so as to inherit their parents */ 
    (*self)->__parent = NULL;
    /* Methods */
#if !defined(NOBJ_METHODS)
    (*self)->string = addStringToJsonObject;
    (*self)->remove = removeFromJsonObject;
#endif
    return *self;
}

/*
 * __open__
 */
__NONNULL__ __attribute__((warn_unused_result)) JsonObject JsonOpen(unsigned char *file) {
    struct stat statbf;
    JsonObject newObj;
    register int L0 __UNUSED__;
    register int fd __UNUSED__;

    if (*file == 0) {
	throwError(EINVAL, newObj);
	return NULL;
    }
    if ((fd = open(file, O_RDWR | O_NONBLOCK | O_NOFOLLOW)) == -1) {
	throwError(errno, newObj);
	return NULL;
    }
    if (fstat(fd, &statbf)) {
	throwError(errno, newObj);
	return NULL;
    }
    if (statbf.st_size < MIN_FILE_BYTES) {
    }
    /* initialize object */
    if (! JsonInit(&newObj)) {
	return NULL;
    }

    if(! readfl(newObj, fd, statbf.st_size)) {
	puts("readfl encountered an error");
	perror("");
    }
    return newObj;
}

/* 
 * __read__
 */
static __inline__ void *readfl(JsonObject obj, int fd, ssize_t sz) {
    JsonString bf;
    register ssize_t rdbytes, L1;

    if (fd < 0 || sz < 0) {
	return NULL;
    }
#if !defined(NOBJ_BUFFERING)
    if (sz > MAX_FILE_BYTES) {
	/* TODO: read block by block ( each block reads MAX_FILE_BYTES bytes )  */
	return NULL;
    }
    if (! (bf = malloc(sz + 1))) {
	throwError(ENOMEM, NULL);
	return NULL;
    }
    obj->__buf = bf;
    obj->__szbuf = sz + 1;
#endif
    errno = 0;
    while (((L1 = read(fd, bf, sz)) && (L1 != -1)) || (errno == EINTR)) {
	/* TODO: update file pointer using seek if EINTR */
	rdbytes = L1;
    }
    if (L1 == -1) {
	throwError(errno, NULL);
	return NULL;
    }
    bf[rdbytes] = 0;
    if (! lexer(obj, bf, sz)) {
	puts("lexer error");
	return NULL;
    }
    return obj;
}

__NONNULL__ static __inline__ ssize_t getKey(JsonString str, JsonString __restrict *bf) {
    register JsonChar c;
    register ssize_t rsz, i;

    if (*str == 0)
	return -1;
    /* get length */
    for (rsz = 0; (c = str[rsz]) && !(c == JS_CH('"')); rsz++)
	PASS;
    if (c == JS_CH('"')) {
	/* clear preceeding whitespace */
	for (i = rsz + 1; (c = str[i]) && isspace(c); i++)
	    PASS;
	/* get key value usually after a ':' */
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
/* set to accept key or value */
#define R_EXP_KEY EXP_VAL
#define R_EXP_VAL EXP_KEY
#define SET_OPFLAG(AT, FL) (void)((AT) = (((AT) & MBLK_CLOSE) | FL))
/* increments buffer pointer */
#define M0V_BUFFER_PTR(bf, at) ((bf) += (at))
/* assert end of value characters */
#define EOVAL(c)							\
    (!(((c) ^ JS_CH(',')) && ((c) ^ JS_CH('\n')) && ((c) ^ JS_CH('}')) && ((c) ^ JS_CH(']'))))

/* 
 * __parser__
 */

__NONNULL__ static __inline__ void *lexer(JsonObject obj, JsonString bf, ssize_t sz) {
    register ssize_t objMtch, arrMtch, nkeys, L2, L3, LINE, CH_AT;
    register JsonChar c;
    uint8_t rswitch;

    JsonString key;
    JsonString value;
    objStack *OBJ_trace;
    JsonObject newObj;

    newObj = NULL;
    OBJ_trace = STACK_init(NULL);
    LINE = objMtch = arrMtch = rswitch = 0;

    for (L2 = 0; (c = bf[L2]); L2++) {
	if ((c == JS_CH('\n') ? LINE++ : 0) || isspace(c))
	    continue;
	switch (c) {
	case JS_CH('{'):
	    if (rswitch & EXP_VAL) {
		/* Initialize a new object */
		if (! JsonInit(&newObj)) {
		    throwError(EINIT, NULL);
		    /* This may be undesirable. IMPLEMENT #5, TODO.md */
		    return obj;
		}
		/* The new object is added to the base object */
		addToJsonObject(obj, key, newObj, JSONOBJECT_TYPE);

		/* We update the base object with the new object and store the base on the stack for reuse after the lifetime of newObj is over (we've read upto a '}') */ 
		if (! pushToStack(&OBJ_trace, obj)) {
		    throwError(ENOMEM, NULL);
		    /* IMPLEMENT #5, TODO.md */
		    return obj;
		}
		obj = newObj;
	    }
	    objMtch += 1;
	    rswitch |= MBLK_OBJ | MBLK_START | EXP_KEY;
	    break;
	case JS_CH('}'):
	    /* IMPLEMENT #6, TODO.md */
	    if ((rswitch & EXP_VAL_R_KEY))
		goto syntaxError;
	    if (objMtch > 1) 
		obj = popFromStack(&OBJ_trace)->stk_obj;
	    rswitch &= MBLK_CLOSE;
	    objMtch -= 1;
	    break;
	case JS_CH(':'):
	    puts("trailing colon");
	    goto syntaxError;
	    break;
	case JS_CH('"'):
	    if (rswitch & EXP_KEY) {
		if ((L3 = getKey(bf + ++L2, &key)) == -1) {
		    goto syntaxError;
		}
		key[L3] = 0;
		M0V_BUFFER_PTR(bf, L2 + L3);
		L2 = -1;
		SET_OPFLAG(rswitch, EXP_VAL);
		//printf("%s: ", key);
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
		if ((value = malloc(L2 + 1)) == NULL) {
		    throwError(ENOMEM, NULL);
		    return NULL;
		}
		memcpy(value, bf, L2);
		M0V_BUFFER_PTR(bf, ++L2);
		value[L2] = 0;
		L2 = -1;
		rswitch &= MBLK_CLOSE;

		obj->string(obj, key, value);
		//puts(value);
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
	    //puts("");
	}
    }
    //printf("%ld\n", LINE);
}
/* 
 * __dump__
 */
__NONNULL__ static ssize_t dump(JsonObject self, ...) {
    printfmt fmt;
#if !defined( NOBJ_BUFFERING)
    dumpbf(self, &fmt);
#else
#endif
}

__NONNULL__ ssize_t dumpbf(JsonObject self, printfmt *fmt) {
    JsonString buf;
    JsonData data;
    size_t szbuf, sztbe;

    szbuf = self->__szbuf;
    if (!self->__buf && !(self->__buf = malloc((szbuf = sizeof(JsonChar) * 1024)))) {
	throwError(ENOMEM, NULL);
	return -1;
    }
    self->__szbuf = szbuf;
    buf = self->__buf;

    *buf++ = JS_CH('{');
    *buf++ = JS_CH('\n');
    szbuf -= 2;

    data = self->__head[0];
    while ((sztbe < self->__hashBkt) || data) {
	if (data == NULL) {
	    data = self->__head[sztbe++];
	    continue;
	}
	do {
	    *buf++ = fmt->indent_char;
	    szbuf--;
	} while (--fmt->indent_width);
	/* TODO: REMOVE ASSUMPTIONS: sz is sufficient and no error */
	buf += snprintf(buf, szbuf - 2, "%s: %s,\n", data->__key, data->__str);
	data = data->__nd;
    }
    *buf++ = JS_CH('}');
    *buf++ = JS_CH('\n');
    *buf = 0;

    szbuf = fprintf(stdout, "%s", (JsonString)self->__buf);
    return szbuf;
}
__NONNULL__ void JsonMemcpy(void *dest, void *src, ssize_t sz) {
    ssize_t chk;
}
void static __inline__ __jsonError__(int errn, char * __restrict line, char * __restrict objf, char *obj) {
}

int main(int argc, char **argv) {
    JVEC_t vec;

    vec = VEC_create(1);    
    return 0;
}

