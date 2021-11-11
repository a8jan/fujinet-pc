#ifndef _COMPAT_INET_
#define _COMPAT_INET_

#if defined(_WIN32)

#include <stdint.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef uint32_t in_addr_t;

#else

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <sys/socket.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* takes in_addr_t as argument */
char *compat_inet_ntoa(in_addr_t in);

#ifdef __cplusplus
}
#endif


#endif // _COMPAT_INET_