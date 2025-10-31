#include "client.h"

void *webdriverMalloc(const size_t size)
{
  void *p;

  if ((p = calloc(1, size)) == NULL)
	{
	  return (void *)WEBDR_EOMEM;
	};

  return p;
}

static __inline__ __attribute__((always_inline)) void *webdriverRealloc(void *p, const size_t size)
{
  void *re;

  if ((re = realloc(p, size)) == NULL)
	{
	  return (void *)WEBDR_EOMEM;
	};
  p = re;
  return re;
}

static __inline__ __attribute__((always_inline)) void webdriverDealloc(void *p)
{
  return free(p);
}

static struct sockaddr_storage setsockaddr6(const uint16_t port, const char *ipaddr, struct sockaddr_storage *saddr)
{

  struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)saddr;

  inet_pton(AF_INET6, ipaddr, &(addr6->sin6_addr));
  addr6->sin6_family = AF_INET6;
  addr6->sin6_port = htons(port);

  return *(struct sockaddr_storage *)addr6;
}

static struct sockaddr_storage setsockaddr(const uint16_t port, const char *ipaddr, struct sockaddr_storage *saddr)
{
  struct sockaddr_in *addr = (struct sockaddr_in *)saddr;

  inet_pton(AF_INET, ipaddr, &(addr->sin_addr));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);

  return *(struct sockaddr_storage *)addr;
}

static __inline__ __attribute__((always_inline)) struct addrinfo *inetmatchf(const int family, struct addrinfo *addr)
{
  while (addr != NULL)
	{
	  if (addr->ai_family == family)
		break;
	  addr = addr->ai_next;
	}
  return addr;
}

static const __inline__ __attribute__((always_inline, pure)) struct Webdriver_Itoabf uitoa(uint64_t n)
{
  struct Webdriver_Itoabf uibf;
  uint8_t *pb, *pf , c;

  pb = pf = uibf.ibf;
  while (n)
	*pb++ = (n - ((n /= 10) * 10)) | 0x30;
  *pb = '\0';
  uibf.len = pb - uibf.ibf;

  while (pf < pb)
	c = *pf, *pf++ = *--pb, *pb = c;

  return uibf;
}

Webdriver_Client webdriverCreateClient(const Webdriver_Config *conf)
{
  Webdriver_Client client;
  char *host;
  int   fd = -1, stat = 0, dobind = true, server, domain, port;

  if (conf)
	{
	  port   = conf->port,   host   = conf->host;
	  domain = conf->domain, server = conf->server;
	}
  else
	host = NULL, port = server = domain = 0;

  // Reserve space for client
  WerrorOccured( webdriverMalloc(WEBDR_BASE_SIZE), client )
	return (void *)WEBDR_EOMEM;

  if (host == NULL || !strcmp(host, "localhost"))
	host = domain == AF_INET6 ? WEBDR_LOCALHOST_I6 : WEBDR_LOCALHOST_I4;

  if (! server)
	{
	  if (port && host)
		JMP(Jmp_cclient);
	  // [fallthrough] Get port/host from getaddrinfo
	  dobind = false;
	}

  /* SERVER */
  {
	struct addrinfo hint, *info, *node;

	memset(&hint, 0, sizeof hint);
	domain = WEBDR_SET_AFNET(hint.ai_family, domain, AF_UNSPEC);
	hint.ai_socktype = WEBDR_SOCKTYPE;
	hint.ai_protocol = WEBDR_PROTOCOL;

	WerrorOccured(getaddrinfo(host, port ? uitoa(port).ibf : NULL, &hint, &info), stat)
	  {
		webdriverDealloc(client);

		return (void *)WEBDR_ADDRINFO;
	  }

	node = info;
	do
	  {
		if (domain && !(node = inetmatchf(domain, info)))
		  {
			/* Unable to find any address of type domain that binds: default to any af_family. This cost a second loop over the struct list
			 * To disable default behaviour, #define WEBDR_STRICT_INET (see socket.h:WEBDR_SET_AFNET)
			 */
			domain = WEBDR_DOMRESET;
			node = info;
		  }

		WerrorOccured(socket(node->ai_family, SOCK_STREAM, IPPROTO_TCP), fd)
		  {
			client->errno__ = errno;
			freeaddrinfo(info);
			webdriverDealloc(client);

			return (void *)WEBDR_EOSOCK;
		  };

		dobind && (
				   stat = bind(fd, (struct sockaddr *)(node->ai_addr), sizeof(struct sockaddr))
				   );
		if (stat == 0 || stat != EADDRINUSE)
		  {
			client->errno__ = errno;

			break;
		  }
		WerrorOccured(close(fd), stat);
		node = node->ai_next;
	  } while (node != NULL);

	if (node != NULL)
	  client->addr__ = *(struct sockaddr_storage *)(node->ai_addr);

	freeaddrinfo(info);

	if (stat != 0)
	  {
		Debug(WEBDR_EOBIND);
		webdriverDealloc(client);
		return (void *)WEBDR_EOBIND;
	  }
	JMP(Jmp_return);
  }

  // CLIENT
  {
	LOCATION(Jmp_cclient);

	client->addr__ = (domain == AF_INET6 ? setsockaddr6
					  : ((domain = AF_INET), setsockaddr))(port, host, &(client->addr__));

	WerrorOccured(socket(domain, SOCK_STREAM, IPPROTO_TCP), fd)
	  {
		client->errno__ = errno;
		webdriverDealloc(client);

		return (void *)WEBDR_EOSOCK;
	  }
  }

  LOCATION(Jmp_return);

  *(int *)(void *)&(client->sock__) = fd;
  return client;
}

void webdriverDestroyClient(Webdriver_Client client)
{
  int fd;

  if (client != NULL)
	{
	  WerrorOccured(close(client->sock__), fd);
	  webdriverUnsetbuf(client);
	  webdriverDealloc(client);
	}
}
void *webdriverSetbuf(Webdriver_Client client, void *buf, size_t size)
{
  if (client->buf__ == NULL) {
	if (buf == NULL)
	  {
		NOT (size) && (size = WEBDR_DEF_MALLOC_SIZE);
		WerrorOccured(webdriverMalloc(size), client->buf__)
		  {
			client->bufsize__ = 0;
			return (void *)WEBDR_EOMEM;
		  }
		client->bufc__ = client->bufsize__ = size;
		client->malloc__ = true;
	  }
	else {
	  client->buf__ = buf, client->bufsize__ = client->bufc__ = size;
	}
	return (void *)WEBDR_SUCCESS;
  }
  return (void *)WEDR_EONOTEMPT;
}

static __attribute__((nonnull)) void *webdriverGrowbuf(Webdriver_Client client)
{
  size_t size = client->bufsize__;

  if (client->malloc__ == false)
	return (void *)WEBDR_EOMEM;

  if (client->buf__ == NULL)
	return webdriverSetbuf(client, NULL, size);

  if (NOT(
		  safeUnsignedSize_Add(size, WEBDR_DEF_MALLOC_GRW, &size)
		  && webdriverRealloc(client->buf__, size)
		  ))
	return (void *)WEBDR_EOMEM;

  client->bufsize__ = size;
  client->bufc__ +=  WEBDR_DEF_MALLOC_GRW;
}

__attribute__((nonnull)) void *webdriverUnsetbuf(Webdriver_Client client)
{
  if (client->buf__ && client->malloc__)
	webdriverDealloc(client->buf__);

  client->buf__ = NULL;
  client->bufsize__ = client->bufc__ = client->malloc__ = 0;

  return (void *)WEBDR_SUCCESS;
}

static __inline__ __attribute__((always_inline, nonnull)) void *strroutine_Join(char * __restrict__ buf, const void * __restrict__ tab, size_t *size, const bool delim)
{
  uintmax_t j, n;
  char *node, **ptab;

  ptab = (void *)tab;
  j = *size;
  while (
		 (node = *ptab++)
		 && (n = strlen(node)) < j
		 )
	{
	  __builtin_memcpy(buf, node, n);
	  buf += n;
	  j -= n;
	}
  *size = j;

  if (node != NULL)
	return (void *)(--ptab);

  if (delim) {
	if (j < 2)
	  return (void *)ptab;
	*size -= WEBDR_APPEND_DELIM(buf, 0);
  }
  return (void *)WEBDR_SUCCESS;
}

static const __inline__ __attribute__((always_inline, nonnull)) void *webdriverAddHttpContent(Webdriver_Client client, void *cc)
{
  do
	{
	  // if buffer was malloc’d, attempt to join until success or memory error
	  cc = strroutine_Join(client->buf__ + (client->bufsize__ - client->bufc__), cc, &(client->bufc__), true);
	} while (cc != (void *)WEBDR_SUCCESS && webdriverGrowbuf(client) != (void *)WEBDR_EOMEM);

  return (cc == (void *)WEBDR_SUCCESS ? (void *)WEBDR_SUCCESS : (void *)WEBDR_EOMEM);
}

__attribute__((nonnull)) const void *webdriverSetHttpCmd(Webdriver_Client client, const int method, const void *cmd)
{
  const void *tab[6] = {
						WDHttpMethodStr[method],
						" ",
						cmd,
						" ",
						WEDR_HTTP_PROTOCOL,
						NULL
  };

  if (NOT (webdriverSupportedMethods(method)) )
	return (void *)WEBDR_EOMETHOD;

  return webdriverAddHttpContent(client, tab);
}

__attribute__((nonnull)) const void *webdriverAddHttpHeader(Webdriver_Client client, const void * __restrict field, const void * __restrict value)
{
  const void *tab[4] = {
						field,
						":",
						value,
						NULL
  };
  return webdriverAddHttpContent(client, tab);
}

__attribute__((nonnull)) void *webdriverRawHttpHeader(Webdriver_Client client, const void *content) {
  const size_t n = strlen(content);

  if (NOT (n))
	return (void *)WEBDR_SUCCESS;
  if (client->bufc__ < n)
	return (void *)WEBDR_INCOMPLETE;

  memcpy(client->buf__ + (client->bufsize__ - client->bufc__), content, n);
  return (void *)WEBDR_SUCCESS;
}

__attribute__((nonnull)) void webdriverShowHttpHeaders(Webdriver_Client client) {
  return (void)(
				(client->buf__) && puts(client->buf__)
				);
}
__attribute__((nonnull)) size_t webdriverBufferUsed(Webdriver_Client client) {
  return (client->bufsize__ - client->bufc__);
}

#define Chunk(chunk) (*(void **)(chunk))
#define chunkSize(chunk) ((uint16_t *)(void *)chunk)[-1]
typedef struct Webdriver_TMemoryPool__ * Webdriver_TMemoryPool;
typedef uint8_t Byte;

struct Webdriver_TMemoryPool__ {
  Byte *__tp__;
  Byte *__mp__;
  void *__free__;
  void *__tail__;
  Webdriver_TMemoryPool __next__;
};

static __inline__ __attribute__((always_inline)) void *webdriverMemoryPool(void)
{
  Webdriver_TMemoryPool mempool;

  if (NOT
	  ((mempool = webdriverMalloc(  webdriverMemoryPoolSize )))
	  )
	{
	  errno = ENOMEM;
	  return (void *)WEBDR_EOMEM;
	}

  mempool->__tp__ = (Byte *)mempool + ( webdriverMemoryPoolSize - 1);
  mempool->__mp__ = (Byte *)mempool +  webdriverSizeofMemoryPool;
  mempool->__free__ = NULL;
  mempool->__tail__ = NULL;
  mempool->__next__ = NULL;

  return mempool;
}

static __inline__ __attribute__((always_inline)) void *free_firstFit(Webdriver_TMemoryPool mempool, const uint16_t size) {
  void **cnode, **pnode = NULL;

   for (cnode = mempool->__free__; cnode && (chunkSize(cnode) < size); pnode = cnode, cnode = *cnode);

  if (cnode)
	{
	  if (chunkSize(cnode) > ((uint32_t)size * 2.5))
		{
		  // TODO: Split into chunk; keep half in free list and return the other
		}
	  pnode == NULL ? (mempool->__free__ = *cnode)
	  : cnode == mempool->__tail__ ? (mempool->__tail__ = pnode) : (*pnode = *cnode);
	}
  return cnode;
}

#ifdef WEBDR_MEMPOOL_LINK
static __inline__ __attribute__((always_inline)) pool_firstFit(const Webdriver_TMemoryPool mempool, const uint16_t size) {
  const void *endp = mempool->__tp__ - size - webdriverMemoryPoolMetaSize;

  for (Webdriver_TMemoryPool cpool = mempool; cpool && (cpool->__mp__ > endp); cpool = cpool->__next__);

  return cpool;
}
#else
#define pool_firstFit(mempool, size) (mempool && (mempool)->__mp__ > ((mempool)->__tp__ - size - webdriverMemoryPoolMetaSize) ? NULL : (mempool))
#endif

static __inline__ __attribute__((nonnull, always_inline)) void *webdriverMemoryPoolGetUnlocked(Webdriver_TMemoryPool mempool, uint16_t size)
{
  Webdriver_TMemoryPool cpool; void *ctmp __attribute__((unused)) = NULL;

  // Any fit in the pool?
  cpool = pool_firstFit(mempool, size);
  if ( cpool ) {
	ctmp = cpool->__mp__  += webdriverMemoryPoolMetaSize;
	cpool->__mp__  += size;
	chunkSize(ctmp) = size;

	return ctmp;
  }

  // Any fit in the free list?
  if ( (ctmp = free_firstFit(mempool, size)) )
	  return memset(ctmp, 0, chunkSize(ctmp));

#if 0
  // TODO: extend/Add a new pool
  cpool = webdriverMemoryPoolGrow(mempool, 0, 0);
#endif

  errno = ENOMEM;
  return (void *)WEBDR_EOMEM;
}

static __inline__ __attribute__((always_inline, nonnull)) void *webdriverMemoryPoolGet(Webdriver_TMemoryPool mempool, size_t size)
{
  void *memp;

  if (webdriverMemoryPoolMaxAlloc < size)
	return (void *)WEBDR_EOMEM;

  (size < webdriverMemoryPoolMinAlloc) && (size = alignUp(size, webdriverMemoryPoolMinAlloc));

  _LOCK();
  memp = webdriverMemoryPoolGetUnlocked(mempool, size);
  _UNLOCK();

  return memp;
}

__attribute__((nonnull)) void webdriverMemoryPoolDeleteChunk(Webdriver_TMemoryPool mempool, void **chunk)
{
  void *chunk__ = *chunk;

  if (NOT (chunk__))
	return;

  if (NOT (mempool->__free__))
	{
	  Chunk(mempool->__free__ = mempool->__tail__ = chunk__) = NULL;
	  return;
  }
  Chunk(mempool->__tail__) = chunk__;
  Chunk(chunk__) = NULL;
  mempool->__tail__ = chunk__;

  *chunk = NULL;
}

__attribute__((nonnull)) void webdriverMemoryPoolDelete(Webdriver_TMemoryPool *mempool)
{
  Webdriver_TMemoryPool cur = *mempool, nxt;

  while (cur != NULL)
	{
	  nxt = cur->__next__;
	  webdriverDealloc(cur);
	  cur = nxt;
  }
  *mempool = NULL;
}

void *webdriverMemoryPoolGrowChunk(Webdriver_TMemoryPool mempool, void **chunk, uint16_t size)
{
  void *chunk__ __attribute__((unused));

  if (size == 0)
	{
	  webdriverMemoryPoolDeleteChunk(mempool, chunk);
	  *chunk = NULL;
	  return (void *)WEBDR_EOMEM;
	}

  if (size <= chunkSize(*chunk))
	{
	  size = chunkSize(*chunk) - size;
	  if ( size )
		{
		  // TODO: if size is alot smaller than chunkSize add a fraction to free
		}
	  memset(*chunk + size, 0, size);

	  return *chunk;
	}

  chunk__ = webdriverMemoryPoolGet(mempool, size);
  if (chunk__ != (void *)WEBDR_EOMEM)
	{
	  memcpy(chunk__, *chunk, size);
	  webdriverMemoryPoolDeleteChunk(mempool, chunk);
	  *chunk = chunk__;
	}

  return chunk__;
}

void webdriverMemoryPoolGrow(Webdriver_TMemoryPool mempool, const uint16_t flags, const uint16_t factor)
{

#ifdef WEBDR_MEMPOOL_EXT
  //MEMPOOL_EXT: extend the current pool
  ASSERT ("UNIMPLEMENTED", false);
#elif defined(WEBDR_MEMPOOL_LINK)
  // WEBDR_MEMPOOL_LINK: link a new pool to current pool
  ASSERT ("UNIMPLEMENTED", false);
#else
  // Pool is fixed (default)
  fprintf(stderr, "Bad request: attempt to resize a fixed pool");
  exit(WEBDR_FAILURE);
#endif

}

typedef struct Webdriver_TObject__ {
  void   *__object__[webdriverObjectArraySize];
  void   *__obnext__;
  uint8_t __obmeta__ [webdriverObjectMetaSize];
} Webdriver_TObject__;
typedef Webdriver_TObject__* Webdriver_TObject;

__attribute__((noinline)) Webdriver_TObject webdriverObject(Webdriver_TMemoryPool mempool)
{
  Webdriver_TObject object = webdriverMemoryPoolGet(mempool, sizeof (Webdriver_TObject__));

  if ( object )
	{
	 memset(object->__obmeta__, 0, webdriverObjectMetaSize);
	 object->__obnext__ = NULL;
	}

  return object;
}

static __inline__ __attribute__((always_inline, pure)) int webdriverObjectKeyHash13(const char *key)
{
  uint32_t mask = 0, e = __builtin_strlen(key);

  if (e < 4)
	{
	  // Something different for keys less than 4
	  return -1;
	}
  mask = *(uint16_t *)key;
  key += e - 2;
  mask ^= ((uint32_t)*(uint16_t *)key << 16);

  return ((~mask ^ 0x5a2b0f0f) + (mask - (0xe2e3e3e3 * key[e-1]))) % 13;
}
