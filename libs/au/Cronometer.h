/* ****************************************************************************
 *
 * FILE                     Cronometer.h - Cronometer to meassure the ellapsed time
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            April 2011
 *
 */

#ifndef SAMSON_CRONOMETER_H
#define SAMSON_CRONOMETER_H

#include <stdio.h>             /* sprintf */
#include <sys/time.h>	       // struct timeval
#include <string>               // std::string

namespace au {

    class Cronometer
    {
        time_t t;
        
    public:
        
        Cronometer();

        // Reset the count to 0
        void reset();
        
        // Get the time difference since the last reset ( or creation ) in second
        time_t diffTimeInSeconds();
        
        // Get a string with the time ellapsed since last reset or creation
        std::string str();
        
    };
    
    class CronometerSystem
    {
        Cronometer c;
        int seconds;
        bool running;
        
    public:

        CronometerSystem();
        
        void start();
        void stop();
        
        // Reset the count to 0
        void reset();
        
        // Get a string with the time ellapsed since last reset or creation
        std::string str();
        
        int getSeconds();
        
    };   
    
}

#endif
