#ifndef HTTP_CLIENT
#define HTTP_CLIENT

#if defined(WIN32) || defined(_MSC_VER) || defined(MINGW)
    #define WIN_SUPPORT
    #incliude <winsocket.h>
#elif defined(__unix__) || defined(__linux__) || defined(linux)
    #define UNIX_SUPPORT
    #include <netdb.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
#else
    #error "no support for this system"
#endif
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>


#define WEBDR_PROTOCOL IPPROTO_TCP
#define WEBDR_SOCKTYPE SOCK_STREAM

#define WEDR_HTTP_PROTOCOL "HTTP/1.1"
#define WEBDR_LOCALHOST_I6 "::1"
#define WEBDR_LOCALHOST_I4 "127.0.0.1"

// Allow domain hint to be used strictly as the domain type
#ifdef WEBDR_STRICT_INET
// disallow fallback loop to generic in address searching
    #define WEBDR_SET_AFNET(af, hint, def) (((af) = (hint)), WEBDR_DOMRESET)
#else
#define WEBDR_SET_AFNET(af, hint, def)\
  ((af) = (def), (hint) != 0 ? (hint) : WEBDR_DOMRESET)
#endif

#define WEBDR_APPEND_DELIM(buf, n) ((buf[n] = de.CR), (buf[n + 1] = de.LF), 2)

#define ERROR_socket(...)      < 0) && Debug(WEBDR_EOSOCK
#define ERROR_bind(...)        < 0) && Debug(WEBDR_EOBIND
#define ERROR_close(...)       < 0) && Debug(WEBDR_CLOSE
#define ERROR_WebdriverMalloc(...)  == NULL) && Debug(WEBDR_EOMEM
#define ERROR_getaddrinfo(...) != 0) && Debug(0, gai_strerror(stat)

#define WerrorOccured(funcall, stat, ...)			\
  if (((stat = funcall) ERROR_##funcall, __VA_ARGS__))

  #ifdef NDEBUG
    #define Debug(...) true
#else
    #define Debug(symbol, ...) fprintf(stderr, "%s\n", "http error")
#endif
#define Webdriver_strerror(err) "error"

#define JMP(label) goto label
#define LOCATION(label) label: (void)0
#define NOT(e) !(e)

typedef struct {
  char *header;
  char *body;
} Wedbriver_MsgContent;

typedef struct Webdriver_Client__ Webdriver_Client__;
typedef struct Webdriver_Client__ * Webdriver_Client;

struct Webdriver_Client__ {
  struct sockaddr_storage addr__;
  char *buf__;
  size_t bufsize__;
  size_t bufc__;
  int errno__;
  const int sock__;
  _Bool memrelease__;
};

#define WEBDR_BASE_SIZE sizeof (Webdriver_Client__)

typedef struct {
  char    *host;
  uint16_t port;
  uint16_t domain;
  _Bool    server;
} Webdriver_Config;

struct Webdriver_Itoabf
{
  uint8_t ibf[22];
  uint8_t len;
};

enum {
      MAX_NERROR     = 10,
      MIN_NERROR     = 0,
      WEBDR_DOMRESET = 0,
      WEBDR_EOMEM    = 0,
      WEBDR_EOSOCK,
      WEBDR_EOBIND,
      WEBDR_CLOSE,
      WEBDR_ADDRINFO,
      WEBDR_EOMETHOD,
      WEDR_EONOTEMPT, // attempt to reset buffer already allocated
      WEBDR_SUCCESS,
      WEBDR_FAILURE,
};

enum Webdriver_httpMethodSupport {
	     GET,
	     POST,
	     DELETE
} WDHttpMethod;

enum de {
      CR = '\r',
      LF = '\n',
      SP = ' ',
      TAB = 0,
      EOC = '\0',
      EOF
}
static const WDHttpMethodStr[3] = {
					   [WDHttpMethod.GET]    = "GET",
					   [WDHttpMethod.POST]   = "POST",
					   [WDHttpMethod.DELETE] = "DELETE"
};

Webdriver_Client WebdriverCreateClient( const Webdriver_Config * );
void             WebdriverDestroyClient( Webdriver_Client );
_Bool            WebdriverSupportedMethods( const int );
_Bool            WebdriverError( const void * );
void *           WebdriverPerror( const void * )

   extern const __inline__ __attribute__((always_inline, pure)) _Bool WebdriverSupportedMethods(const int method) {
  return method >= WdHttpMethod.GET && method <= WDHttpMethod.DELETE;
}

extern const __inline__ __attribute__((always_inline)) _Bool WebdriverError(const void *perr) {

  return (const uintptr_t)no > MIN_NERROR && (const uintptr_t)no < MAX_NERROR;
}

extern const __inline__ __attribute__((always_inline)) void *WebdriverPerror(const void *perr) {
  // Unimplemented
  return NULL;
}

//HTTP CLIENT
#endif
