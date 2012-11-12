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

#include "TrafficStatistics.h"

namespace stream_connector {
    
    TrafficStatistics::TrafficStatistics() : token("TrafficStatistics" )
    {
        
    }
    
    void TrafficStatistics::push_input( size_t size )
    {
        au::TokenTaker tt(&token);
        input_rate.push( size );
    }
    
    void TrafficStatistics::push_output( size_t size )
    {
        au::TokenTaker tt(&token);
        output_rate.push( size );
    }
    
    size_t TrafficStatistics::get_input_total()
    {
        au::TokenTaker tt( &token );
        return input_rate.getTotalSize();
    }
    
    size_t TrafficStatistics::get_output_total()
    {
        au::TokenTaker tt( &token );
        return output_rate.getTotalSize();
    }
    
    size_t TrafficStatistics::get_input_rate()
    {
        au::TokenTaker tt( &token );
        return input_rate.getRate();
    }
    
    size_t TrafficStatistics::get_output_rate()
    {
        au::TokenTaker tt( &token );
        return output_rate.getRate();
    }
    
}