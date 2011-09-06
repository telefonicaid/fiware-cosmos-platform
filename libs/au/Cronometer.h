
/* ****************************************************************************
 *
 * FILE            Cronometer
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Easy to use class to cound how much seconds something happened.
 *      Used in situations where time-precision is not required ( seconds are ok)
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

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
