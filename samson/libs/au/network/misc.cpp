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


