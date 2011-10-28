
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

    
    
}