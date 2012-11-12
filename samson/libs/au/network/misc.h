/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#ifndef _H_AU_NETWORK_MISM
#define _H_AU_NETWORK_MISM

#include <string>

#include "au/string.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

namespace au {

    // ip2string - convert integer ip address to string
    void ip2string(int ip, char* ipString, int ipStringLen);
    
}


#endif
