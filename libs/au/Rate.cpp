
#include "au/string.h"
#include "Rate.h"        // au::Rate

namespace au {
    
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
        return total_size / max_time;
    }
    
    size_t RateItem::getTotalSize( )
    {
        return total_size;
    }
    
#pragma Rate
    
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
    
}