



#ifndef _H_SAMSON_NETWORK_MISM
#define _H_SAMSON_NETWORK_MISM

#include <string>

#include "au/string.h"
#include "au/Token.h"
#include "au/TokenTaker.h"

#include "samson/common/status.h"

namespace samson {

    class Packet;
    
    // ip2string - convert integer ip address to string
    void ip2string(int ip, char* ipString, int ipStringLen);
    
    
}


#endif
