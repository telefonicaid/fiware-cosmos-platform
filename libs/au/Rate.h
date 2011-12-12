
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
#include <math.h>

#include "au/xml.h"
#include "au/Cronometer.h"
#include "au/string.h"

#include "au/namespace.h"


NAMESPACE_BEGIN(au)


/**
 RateValue is a single sample when monitoring a particular rate in bytes/sec
 */
typedef struct
{
    time_t time;
    size_t value;
} RateValue;

/**
 RateItem: Collection of RateValue's to compute a particular rate
 */

class RateItem
{
    std::list<RateValue> values;        // List of values for the last minute
    int max_time;                       // Maximum time to accumulate samples
    
    size_t total_size;                  // total size inside the "buffer"
    
public:
    
    RateItem( int _max_time );
    
    // Add a new sample
    void push( size_t value );
    
    // Review old samples
    void review();
    
    // Get information about the rate
    int getNumOperation();
    size_t getRate( );
    size_t getTotalSize( );
    
};

/*
 Complete class to keep control of all the rate in a particular system
 It keeps all the samples of the last minute and the last hour
 */

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
    
    // Get the last_minute rate
    size_t getLastMinuteRate();
    
};


/**
 Simplifier version of the Rate for only the last minute
 */

class SimpleRate
{
    RateItem last_minute;
    size_t total_size;
    
public:
    
    // Constructor
    SimpleRate();
    
    // Puch a new value into the rate-system
    void push( size_t value );
    
    // Get the last_minute rate
    size_t getLastMinuteRate();
    
    // Get some information in xml format
    void getInfo( std::ostringstream &output );
    
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

NAMESPACE_BEGIN(rate)

class Rate
{
   size_t total_size;   // Total number of bytes
   size_t total_num;       // Total number of hits
   
   double rate;         // Aprox Rate in bytes/sec
   double rate_hits;    // Aprox Rate of hits/sec

   au::Cronometer cronometer;
   au::Cronometer global_cronometer;
    
public:
    
    Rate()
    {
        total_size = 0;
        total_num = 0;
        
        rate = 0;
        rate_hits = 0;		
    }
    
    void push( size_t size )
    {
        update_values();

        // Add to rates
        rate += size;
        rate_hits += 1;
        
        // Add new values
        total_size += size;
        total_num++;
    }
    
    std::string str()
    {
        update_values();
        return au::str("[ Currently %s %s ] [ Accumulated in %s %s with %s ]" 
                       , au::str( rate_hits , "hits/s" ).c_str() 
                       , au::str( rate , "B/s" ).c_str() 
                       , au::time_string( global_cronometer.diffTime() ).c_str()
                       , au::str( total_num , "hits" ).c_str() 
                       , au::str( total_size , "B" ).c_str() 
                       );
    }
    
    size_t getTotalNumberOfHits()
    {
        return total_num;
    }
    size_t getTotalSize()
    {
        return total_size;
    }
    
    double getGlobalRate()
    {
        double time = global_cronometer.diffTime();
        if( time <= 0 )
            time = 1;
        
        return ((double) total_size) / time;
    }
    
private:
    
    void update_values()
    {
        // Get time diferente since last update and reset cronometer
        double time_diff = cronometer.diffTimeAndReset(); 
        
        rate = rate * pow( 0.99666666  , time_diff );
        rate_hits = rate_hits * pow( 0.99666666  , time_diff );
    }

    
};

NAMESPACE_END


NAMESPACE_END

#endif
