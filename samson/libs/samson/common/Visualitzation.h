/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */




#ifndef SAMSON_VISUALITZATION_H
#define SAMSON_VISUALITZATION_H

/* ****************************************************************************
 *
 * FILE                     Visualitzation.h
 *
 * DESCRIPTION              Information about what to visualize 
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            02/2012
 *
 */

#include <string>

#include "au/Environment.h"

namespace samson
{

    // Main visual options
    
    typedef enum 
    { 
        normal,        // Normal visualitzation
        properties,    // Properties ( queues and stream operations )
        rates,         // Total and current reate ( queues , connections )
        blocks,        // Detailes block information ( queues )
        running,       // Operations being executed ( stream_operations )
        in,            // total and current rate in ( stream_operations )
        out,           // total and current rate in ( stream_operations )
        disk,          // Disk activity ( ls_workers )
        engine,        // Engine activity ( ls_workers )
        verbose,
        verbose2,
        verbose3,
        all,
        stream_block,        // Mode only used in show_block_stream
        stream_block_rates    // Mode only used in show_block_stream
    } VisualitzationOptions;

    
    
    // All informaiton required for visualitzation
    
    class Visualization
    {
        
    public:
        
        VisualitzationOptions options;
        std::string pattern;

        // Environemnt properties
        au::Environment environment;
        
    };

    // Simplified match 
    bool match( std::string pattern , std::string name );

}


#endif