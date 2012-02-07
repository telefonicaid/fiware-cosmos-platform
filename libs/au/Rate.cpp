
#include "au/string.h"
#include "Rate.h"        // au::Rate

NAMESPACE_BEGIN(au)

RateItem::RateItem( int _max_time )
{
    total_size = 0;
    max_time = _max_time;
}

// Puch a new value into the rate-system
void RateItem::push( size_t value )
{
    RateValue rv;
    rv.time = time(NULL);
    rv.value = value;
    
    values.push_back(rv);
    total_size+= value;
}

void RateItem::review()
{
    time_t t =  time(NULL);
    
    
    while( true )
    {
        if( values.size() ==0 )
            return;
        
        if ( ( t - values.front().time ) < max_time )
            return;
        
        total_size -= values.front().value;
        values.pop_front();
    }
    
}

int RateItem::getNumOperation()
{
    return values.size();
}

size_t RateItem::getRate( )
{
    if( values.size() == 0)
        return 0;
    
    // Compute the time since the last sample....
    time_t t =  time(NULL);
    size_t _max_time = t - values.back().time;
    
    // Not consider times bellow 10% of the max time
    if( _max_time < (size_t) (max_time/10) )
        _max_time = (max_time/10);
    
    return total_size / max_time;
}

size_t RateItem::getTotalSize( )
{
    return total_size;
}

#pragma mark Rate

// Puch a new value into the rate-system
void Rate::push( size_t value )
{
    last_minute.push( value );
    last_hour.push( value );
}

// Get a string describing the rate
std::string Rate::str()
{
    last_minute.review();
    last_hour.review();
    
    return au::str( "Last minute: [ %s ] %s || Last hour %s" , 
                   au::str( last_minute.getNumOperation() ,"Ops").c_str() , 
                   au::str( last_minute.getRate() , "Bps" ).c_str(),
                   au::str( last_hour.getTotalSize() , "B" ).c_str() );
}

size_t Rate::getLastMinuteRate()
{
    last_minute.review();
    return last_minute.getRate();
}

#pragma mark SimpleRate         

SimpleRate::SimpleRate() : last_minute(60)
{
    total_size = 0;
}

// Puch a new value into the rate-system
void SimpleRate::push( size_t value )
{
    last_minute.push( value );
    total_size += value;
}

// Get the last_minute rate
size_t SimpleRate::getLastMinuteRate()
{
    last_minute.review();
    return last_minute.getRate();
}

void SimpleRate::getInfo( std::ostringstream &output )
{
    last_minute.review();
    
    au::xml_simple(output , "size" , total_size );
    au::xml_simple(output , "rate" , last_minute.getRate() );
}        


NAMESPACE_BEGIN(rate)

Rate::Rate()
{
    total_size = 0;
    total_num = 0;
    
    rate = 0;
    rate_hits = 0;		
    
    factor = 0.99666666; // 300 seconds
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
                   , au::time_string( cronometer.diffTime() ).c_str()
                   , au::time_string( global_cronometer.diffTime() ).c_str()
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
