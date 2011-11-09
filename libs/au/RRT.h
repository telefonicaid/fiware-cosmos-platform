
/* ****************************************************************************
 *
 * FILE            RRT
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Class used to mantain samples for monitorization
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/


#ifndef _H_AU_RRT
#define _H_AU_RRT

#include <cstring>
#include <string>    // std::String
#include <time.h>
#include <list>

#include "au/xml.h"

namespace au
{
    
    
    
    template <class T,int n>
    class RRT
    {
        
    public:
        
        std::list<T> elements;
        
        RRT()
        {
            while( elements.size() < n )
                elements.push_back( 0 );
        }
        
        void push( T value )
        {
            elements.push_back( value );
            
            while (elements.size() > n)
                elements.pop_front();
        }
        
        
        T get( int p )
        {
            
            int c = 0;
            typename std::list<T>::iterator it;
            for ( it = elements.begin() ; it != elements.end() ; it++ )
            {
                if( c == p)
                    return *it;
                c++;
            }
            
            return 0;
        }
        
        T average()
        {
            T total;
            typename std::list<T>::iterator it_elements;
            for (it_elements = elements.begin() ; it_elements  != elements.end() ; it_elements++)
                total += *it_elements;
            
            return total / elements.size();
        }
        
        
    };   
    
    
    template <class T>
    class ValueCollection
    {
        // Values forsize    
        au::RRT<T ,60> samples_last_minute;
        au::RRT<T ,60> samples_last_hour;
        au::RRT<T ,24> samples_last_day;
        
        size_t samples;
        
    public:
        
        ValueCollection()
        {
            samples = 0;
        }
        
        void push( T value )
        {
            // Last minute
            samples_last_minute.push( value );
            
            if( samples > 0 )
                if( (samples % 60) == 0 )
                    samples_last_hour.push( samples_last_minute.average() );
            
            if( samples > 0 )
                if( ( samples % 3600 ) == 0 )
                    samples_last_day.push( samples_last_hour.average() );
            
            samples++;
         
        }
        
        // For ploting
        int getNumSamples()
        {
            return 24 + 60 + 60;
        }
        
        T getSample( int i )
        {
            // Out of range...
            if( i< 0)
                return 0;
            
            if( i < 24 )
                return samples_last_day.get( i );
            
            i-= 24;

            if( i < 60 )
                return samples_last_hour.get( i );

            i-= 60;

            if( i < 60 )
                return samples_last_minute.get( i );
            
            // Out of range...
            return 0;
        }
                    
        
    };
    
    
    template <class T>
    class ContinuousValueCollection : public ValueCollection<T>
    {
        T value;    // Current value
        
        
    public:
        
        void set( T _value )
        {
            value = _value;
        }
        
        void takeSample()
        {
            push( value );
        }
        
    };
    
}

#endif
