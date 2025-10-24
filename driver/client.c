#include "client.h"

void *WebdriverMalloc(const size_t size)
{
  void *p;

  if ((p = calloc(1, size)) == NULL)
    {
      return (void *)WEBDR_EOMEM;
    };

  return p;
}
static __inline__ __attribute__((always_inline)) void WebdriverDealloc(void *p)
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

Webdriver_Client WebdriverCreateClient(const Webdriver_Config *conf)
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

  // Reserve space for wdClient
  WerrorOccured( WebdriverMalloc(WEBDR_BASE_SIZE), client)
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
	WebdriverDealloc(client);

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
	    WebdriverDealloc(client);

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
	WebdriverDealloc(client);
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
	WebdriverDealloc(client);

	return (void *)WEBDR_EOSOCK;
      }
  }

  LOCATION(Jmp_return);

  *(int *)(void *)&(client->sock__) = fd;
  client->memrelease__ = false;
  return client;
}

void WebdriverDestroyClient(Webdriver_Client client)
{
  int fd;

  if (client != NULL)
    {
      WerrorOccured(close(client->sock__), fd);
      WebdriverUnsetbuf(client);
      WebdriverDealloc(client);
    }
}
__attribute__((nonnull)) WebdriverSetbuf(Webdriver_Client client, const char *buf, const size_t size) {
  if (client->buf__ != NULL) {
    if (buf == NULL) 
      {
	client->bufsize__ = size ? size : WEDR_DEF_MALLOC_SIZE;
	WerrorOccured(WebdriverMalloc(client->bufsize__), client->buf__)
	  {
	    client->bufsize__ = 0;
	    return (void *)WEBDR_EOMEM;
	  }
	client->bufc__ = client->bufsize__;
	client->memrelease__ = true;
      }
    else {
      client->buf__ = buf, client->bufsize__ = client->bufc__ = size;
    }
    return (void *)WEDR_SUCCESS;
  }
  return (void *)WEDR_EONOTEMPT;
}

__attribute__((nonnull)) void WebdriverUnsetbuf(Webdriver_Client client) {
  if (client->buf__ && client->memrelease__)
    WebdriverDealloc(client->buf__);
  client->buf__ = NULL;
  client->bufsize__ = 0;
}

static __inline__ __attribute__((nonnull)) ssize_t strroutine_JoinTab(void * __restrict__ buf, void * __restrict__ tab, size_t *size, const int sep) {
  size_t j, b;
  char *ptabI, **ptab;

  ptab = tab;
  for (j = 0, b = *size, ptabI = *ptab++;
       (ptabI != NULL) && (b > 0); j++, b--)
    {
      (void)(
	     NOT(buf[j] = ptabI[j]) && ((buf[j] = sep), (ptabI = *ptab++))
	     );
    }
  if (b < 2)
    return -1;

  return ( *size = b - WEBDR_APPEND_DELIM(buf, j) );
}
static __inline__ __attribute__((nonnull)) void *WebdriverSetHttpCmd(Webdriver_Client client, const int method, const char *__restrict cmd) {
  size_t j, b;
  char *tab[4] = {NULL}, tptr, bptr;

  if (! WebdriverSupportedMethods(method))
    return (void *)WEBDR_EOMETHOD;

  tab[0] = WDHttpMethodStr[method];
  tab[1] = cmd;
  tab[2] = WEDR_HTTP_PROTOCOL;

  if (strroutine_JoinTab(client->buf__, tab, &(client->bufc__), de.SP) < 0)
    return (void *)WEBDR_INCOMPLETE;

  return (void *)WEDR_SUCCESS;
}
static __inline__ __attribute__((nonnull)) void WebdriverAddHttpHeader(Webdriver_Client, const char * __restrict field, const char * __restrict value) {
  
}
