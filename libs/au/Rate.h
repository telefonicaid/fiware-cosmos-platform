
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
   size_t total_num;    // Total number of hits
   
   double rate;         // Aprox Rate in bytes/sec
   double rate_hits;    // Aprox Rate of hits/sec

   au::Cronometer cronometer;
   au::Cronometer global_cronometer;
    
    double factor; 
    
public:
    
    Rate();
    void push( size_t size );
    std::string str();
    std::string strAccumulatedAndRate();

    size_t getTotalNumberOfHits();
    size_t getTotalSize();
    
    double getRate();
    double getHitRate();
    double getGlobalRate();
    
    void setTimeLength( double t );

    void update_values();
    
    void getInfo( std::ostringstream &output )
    {
        au::xml_simple(output , "size" , getTotalSize() );
        au::xml_simple(output , "rate" , getRate() );
    }        
    
private:
    
    double transformRate( double value );
    double correctedRate( double value );
    
};

NAMESPACE_END


NAMESPACE_END

#endif
