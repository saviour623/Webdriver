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

enum {
	  OBJECT,
	  ARRAY,
	  STRING,
	  NUMBER,
	  BOOLEAN
};

#define webdriverObjectArraySize 16
#define webdriverObjecAllocSize 1024

struct webdriver_MemoryPool {
  void *__memory__;
  void *__mp__;
  void *__free__;
  void *__next__;
} mempool__;

struct webdriver_TObject {
  void *ObjectCon[ webdriverObjectArraySize ];
  struct webdriver_Store __malloc__;
  uint8_t __meta__;
};

__attribute__((nonnull)) void webdriverObject(webdriver_TObject *object)
{
  object->__malloc__ = webdriverMemoryPoolGet(webdriverObjectAllocSize);
  object->__meta__  |= OBJECT;
}

#define webdriverMemoryPoolSize 32768
#define webdriverMemoryPoolMinAlloc 8
#define webdriverMemoryPoolMaxAlloc 63355
#define alignUp(n, p2) (((n) + ((p2) - 1)) & ~((p2) - 1))

static __inline__ __attribute__((nonnull, always_inline)) void webdriverMemoryPool(void)
{

  ASSERT ("Bad call: Pool is non-empty", mempool__.__memory__ == NULL);
  ASSERT (WerrorOccurred(webdriverMalloc(), mempool__.__memory__));
  mempool__.__mp__   = mempool__.__memory__;
  mempool__.__free__ = NULL;
  mempool__.__next__ = NULL;
}

void *webdriverMemoryPoolGet(uint16_t size)
{
  if (mempool__.__free__)
	{
	// Transverse;
	}
  (size < webdriverMemoryPoolMinAlloc) && (size = alignUp(size));
  if ((ptrdiff_t)(mempool__.__mp__ - mempool__.__memory__) > allocs)
}

void *webdriverMemoryPoolDelete(webdriver_TObject object)
{
}

void *webdriverMemorypoolGrow(webdriver_TObject)
{
}
