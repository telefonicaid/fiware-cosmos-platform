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

#include "common.h" // Own interface

namespace stream_connector {
    
    const char * str_ConnectionType( ConnectionType type )
    {
        switch (type) {
            case connection_input:
                return "Input";
            case connection_output:
                return "Output";
        }
        return "Unknown";
    }
    
}
