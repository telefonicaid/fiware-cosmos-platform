

/* ****************************************************************************
 *
 * FILE            OnOffMonitor.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 * Classes used to monitor and on-off system determining how much time it is in each state
 *
 * COPYRIGTH       Copyright 2012 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/


#ifndef _H_AU_ONOFFMONITOR
#define _H_AU_ONOFFMONITOR

#include <string>

#include "logMsg/logMsg.h"


namespace au 
{
    // Class used to monitor a on-off system ( % time in each state )
    class OnOffMonitor
    {
        
        bool on; // Flag to determine current position
        
        au::Cronometer cronometer;
        
        double on_time;
        double off_time;
        
        double f; // Factor for the update
        
    public:
        
        OnOffMonitor()
        {
            on = false; // By default it is initially false
            on_time = 0;
            off_time = 0;
            
            double time_span = 30; // Last 30 secs
            f = ( time_span - 1 ) / time_span;
            
        }
        
        double get_on_time()
        {
            return on_time;
        }
        
        double get_off_time()
        {
            return off_time;
        }
        
        void set_on( bool _on )
        {
            double t = cronometer.diffTime();
            
            if( on )
                on_time = on_time += t;
            else
                off_time = off_time += t;
            
            // Forgetting factor
            off_time *= pow( f , t ); 
            on_time *= pow( f , t ); 
            
            // Reset cronometer
            cronometer.reset();
            
            // Change state if any
            on = _on;
        }
        
        double getActivity()
        {
            double t = cronometer.diffTime();
            double _on_time = on_time;
            double _off_time = off_time;
            if( on )
                _on_time += t;
            else
                _off_time += t;
            
            if( ( on_time + off_time ) == 0 )
                return 0;
            else
                return on_time / ( on_time + off_time );
        }
        
        
    };
    
}

#endif