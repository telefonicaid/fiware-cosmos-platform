
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
