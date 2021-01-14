// #include <lwip/netdb.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "fnDNS.h"
#include "../../include/debug.h"


// Return a single IP4 address given a hostname
in_addr_t get_ip4_addr_by_name(const char *hostname)
{
    in_addr_t result = IPADDR_NONE;

    #ifdef DEBUG
    Debug_printf("Resolving hostname \"%s\"\n", hostname);
    #endif
    struct hostent *info = gethostbyname(hostname);

    if(info == nullptr)
    {
        #ifdef DEBUG
        Debug_println("Name failed to resolve");
        #endif
    }
    else
    {
        if(info->h_addr_list[0] != nullptr)
        {
            // struct in_addr vs in_addr_t
            // result = *((in_addr_t*)(info->h_addr_list[0]));
            in_addr in = *((in_addr*)(info->h_addr_list[0]));
            result = in.s_addr;
            #ifdef DEBUG
            // Debug_printf("Resolved to address %s\n", inet_ntoa(result));
            Debug_printf("Resolved to address %s\n", inet_ntoa(in));
            #endif
        }
    }
    return result;
}