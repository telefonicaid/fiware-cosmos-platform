



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
        verbose,
        verbose2,
        verbose3,
        all
    } VisualitzationOptions;

    
    
    // All informaiton required for visualitzation
    
    class Visualization
    {
    public:
        
        VisualitzationOptions options;
        std::string pattern;
        
    };

    // Simplified match 
    bool match( std::string pattern , std::string name );

}


#endif