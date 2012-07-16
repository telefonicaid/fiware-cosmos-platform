
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

#include "au/namespace.h"


NAMESPACE_BEGIN(au)    


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

    void reset()
    {
        elements.clear();
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
class ResizableRRT
{
    
public:
    
    std::list<T> elements;
    int n;
    
    ResizableRRT()
    {
        n=1;
        while( (int)elements.size() < n )
            elements.push_back( 0 );
    }

    void reset()
    {
        elements.clear();
        while( (int)elements.size() < n )
            elements.push_back( 0 );
    }

    void resize(int _n)
    {
        n = _n;
        elements.clear();
        
        while( (int)elements.size() < n )
            elements.push_back( 0 );
    }
   
    void push( T value )
    {
        elements.push_back( value );
        
        while ((int)elements.size() > n)
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

template <class T,int n, int average_length>
class RRTAverage
{
    
public:
    
    std::list<T> elements;
    
    int n_average;
    T average_accumulation;
    
    RRTAverage()
    {
        while( elements.size() < n )
            elements.push_back( 0 );
        
        n_average = 0;
        average_accumulation = 0;
    }
    
    void push( T value )
    {
        
        if( n_average == average_length )
        {
            elements.push_back( average_accumulation / (double) average_length );
            
            while (elements.size() > n)
                elements.pop_front();
            
            // Init the accumulation
            n_average = 0;
            average_accumulation = 0;
        }
        
        // Accumulate sample for the average
        average_accumulation += value;
        n_average++;    
        
    }
    
    int getNumSamples()
    {
        return elements.size();
    }
    
    T getSample( int p )
    {
        // Check limits
        if( p< 0 )
            return 0;
        if( p>(int)elements.size())
            return 0;
        
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
    au::RRTAverage<T ,100,1>    samples_last_seconds;
    //au::RRTAverage<T ,200,60>   samples_last_minutes;
    //au::RRTAverage<T ,200,3600> samples_last_houts;
    
public:
    
    ValueCollection()
    {
    }
    
    void push( T value )
    {
        samples_last_seconds.push( value );
        //samples_last_minutes.push( value );
        //samples_last_houts.push( value );
    }
    
    // For ploting
    int getNumSamples()
    {
        return samples_last_seconds.getNumSamples();
    }
    
    T getSample( int i )
    {
        return samples_last_seconds.getSample(i);
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
        ValueCollection<T>::push( value );
    }
    
};

NAMESPACE_END

#endif
