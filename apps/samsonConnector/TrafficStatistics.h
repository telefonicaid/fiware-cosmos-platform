

#ifndef _H_SAMSON_CONNECTOR_TRAFFIC_STATISTICS
#define _H_SAMSON_CONNECTOR_TRAFFIC_STATISTICS


#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/Rate.h"

namespace samson {
    namespace connector {
        
        class TrafficStatistics
        {
            // Rate statistics
            au::Token token;
            au::rate::Rate input_rate;
            au::rate::Rate output_rate;
            
        public:
            
            TrafficStatistics() : token("TrafficStatistics" )
            {
                
            }
            
            void push_input( size_t size )
            {
                au::TokenTaker tt(&token);
                input_rate.push( size );
            }
            
            void push_output( size_t size )
            {
                au::TokenTaker tt(&token);
                output_rate.push( size );
            }
            
            size_t get_input_total()
            {
                au::TokenTaker tt( &token );
                return input_rate.getTotalSize();
            }
            
            size_t get_output_total()
            {
                au::TokenTaker tt( &token );
                return output_rate.getTotalSize();
            }
            
            size_t get_input_rate()
            {
                au::TokenTaker tt( &token );
                return input_rate.getRate();
            }
            
            size_t get_output_rate()
            {
                au::TokenTaker tt( &token );
                return output_rate.getRate();
            }
            
        };
    }
}

#endif
