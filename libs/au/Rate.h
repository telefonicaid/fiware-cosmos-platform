
/* ****************************************************************************
 *
 * FILE            Rate
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Class used to measure the bandwith of a particular operation
 *  It automatically count the rate in the last minute and last hour
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/


#ifndef _H_AU_RATE
#define _H_AU_RATE

#include <cstring>
#include <string>    // std::String
#include <time.h>
#include <list>

#include "au/xml.h"

namespace au
{
    
    typedef struct
    {
        time_t time;
        size_t value;
    } RateValue;

    class RateItem
    {
        std::list<RateValue> values;        // List of values for the last minute
        int max_time;

        size_t total_size;                  // total size inside the "buffer"
        
    public:
        
        RateItem( int _max_time );
        
        // Add a new sample
        void push( size_t value );

        // Review old samples
        void review();

        // Get information
        int getNumOperation();
        size_t getRate( );
        size_t getTotalSize( );
        
        
    };

    class Rate
    {
        
        RateItem last_minute;
        RateItem last_hour;
                
    public:
        
        Rate() : last_minute(60) , last_hour(3600)
        {
        }
        
        // Puch a new value into the rate-system
        void push( size_t value );
        
        // Get a string describing the rate
        std::string str();
    
        // Set the maximum time
        void setMaxTime( int _max_time );
        
        size_t getLastMinuteRate();
        
    };
    
    
    
    // Simple Rate collection
    
    class SimpleRateCollection
    {
        
        std::string concept;
        
        int num_operations;
        size_t size;
        size_t time;
        
    public:
        
        SimpleRateCollection( std::string _concept )
        {
            concept = _concept;
            num_operations = 0 ;
            size = 0;
            time = 0;
        }
        
        void push( size_t _size , int _time )
        {
            num_operations++;
            size += _size;
            time += _time;
        }
        
        void getInfo( std::ostringstream &output )
        {
            au::xml_open(output, "simple_rate_collection");
            
            au::xml_simple(output , "concept" , concept );
            
            au::xml_simple(output , "num_operations" , num_operations );

            au::xml_simple(output , "size" , size );
            au::xml_simple(output , "time" , time );
            
            au::xml_close(output, "simple_rate_collection");
        }
        
    };
    
    
}
#endif
