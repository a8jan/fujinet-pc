#include "compat_inet.h"

char *compat_inet_ntoa(in_addr_t in)
{
    struct in_addr sin;
    sin.s_addr = in;
    return inet_ntoa(sin);
}
