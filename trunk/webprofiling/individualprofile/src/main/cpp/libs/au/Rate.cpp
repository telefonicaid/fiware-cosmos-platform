
#include "au/string.h"
#include "Rate.h"        // au::Rate

NAMESPACE_BEGIN(au)
NAMESPACE_BEGIN(rate)

Rate::Rate()
{
    total_size = 0;
    total_num = 0;
    
    rate = 0;
    rate_hits = 0;		
    
    setTimeLength( 60 ); // Default behaviour... last minute
}

void Rate::setTimeLength( double t )
{
    factor = ( t - 1 ) / t;
}


void Rate::push( size_t size )
{
    // update the previous values ( forgetting factor... )
    update_values();
    
    // Add to rates
    rate += size;
    rate_hits += 1;
    
    // Add new values
    total_size += size;
    total_num++;
}

std::string Rate::str()
{
   //update_values();
    return au::str("[ Currently %s %s ] [ Last sample %s ] [ Accumulated in %s %s with %s ]" 
                   , au::str( getHitRate()  , "hits/s" ).c_str() 
                   , au::str( getRate() , "B/s" ).c_str() 
                   , au::str_time( cronometer.diffTime() ).c_str()
                   , au::str_time( global_cronometer.diffTime() ).c_str()
                   , au::str_detail( total_num , "hits" ).c_str() 
                   , au::str_detail( total_size , "B" ).c_str() 
                   );
}

std::string Rate::strAccumulatedAndRate()
{
    //update_values();
    return au::str("[ Accumulated %s at %s ]" 
                   , au::str_detail( total_size , "B" ).c_str() 
                   , au::str( getRate() , "B/s" ).c_str() 
                   );
}

size_t Rate::getTotalNumberOfHits()
{
    return total_num;
}
size_t Rate::getTotalSize()
{
    return total_size;
}

double Rate::getRate()
{
    return transformRate( correctedRate(rate) );
}

double Rate::getHitRate()
{
    return transformRate( correctedRate(rate_hits) );
}

double Rate::getGlobalRate()
{
    double time = global_cronometer.diffTime();
    if( time <= 0 )
        time = 1;
    
    return ((double) total_size) / time;
}

void Rate::update_values()
{
    // Get time diferente since last update and reset cronometer
    double time_diff = cronometer.diffTimeAndReset(); 
    
    rate      = rate * pow( factor  , time_diff );
    rate_hits = rate_hits * pow( factor  , time_diff );
}

double Rate::transformRate( double value )
{
    double time_spam = 1.0 / ( 1.0 - factor );
    double time = global_cronometer.diffTime();
    
    if( time == 0 )
        time = 1;
    
    if( time < time_spam )
        return value / time;
    else
        return (1.0-factor)*value;
}

double Rate::correctedRate( double value )
{
    double time_diff = cronometer.diffTime(); 
    
    value = value * pow( factor  , time_diff );
    return value;
}


NAMESPACE_END
NAMESPACE_END
