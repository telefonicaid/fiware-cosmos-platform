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


#ifndef _H_SAMSON_CONNECTOR_TRAFFIC_STATISTICS
#define _H_SAMSON_CONNECTOR_TRAFFIC_STATISTICS


#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/Rate.h"

namespace stream_connector {
    
    class TrafficStatistics
    {
        // Rate statistics
        au::Token token;
        au::rate::Rate input_rate;
        au::rate::Rate output_rate;
        
    public:
        
        TrafficStatistics();        
        
        void push_input( size_t size );
        void push_output( size_t size );

        size_t get_input_total();
        size_t get_output_total();
        size_t get_input_rate();
        size_t get_output_rate();
        
    };
}

#endif
