

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
#include "au/Cronometer.h"

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
        
        OnOffMonitor();

        // Set state
        void set_on( bool _on );

        // Get on %
        double getActivity();

        // Get time for each state
        double get_on_time();
        double get_off_time();

    };
    
}

#endif