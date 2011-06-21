/* ****************************************************************************
 *
 * FILE                     Rate.h - Meassure the rate of something ( bytes / second in a minute )
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            April 2011
 *
 */


#ifndef _H_AU_RATE
#define _H_AU_RATE

#include <cstring>
#include <string>    // std::String
#include <time.h>
#include <list>

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
        
        Rate() : last_minute(20) , last_hour(3600)
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
    
}
#endif
