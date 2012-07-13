

#include "misc.h"  // Own interface

namespace au 
{
    
    void ip2string(int ip, char* ipString, int ipStringLen)
    {
        snprintf(ipString, ipStringLen, "%d.%d.%d.%d",
                 ip & 0xFF,
                 (ip & 0xFF00) >> 8,
                 (ip & 0xFF0000) >> 16,
                 (ip & 0xFF000000) >> 24);
    }
}


