
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

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/namespace.h"

NAMESPACE_BEGIN(au)

class Cronometer
{
    struct timeval t;
    
public:
    
    Cronometer();
    
    // Reset the count to 0
    void reset();
    
    // Get the time difference since the last reset ( or creation ) in second
    time_t diffTimeInSeconds();

    double diffTime();
    double diffTimeAndReset();
    
    // Get a string with the time ellapsed since last reset or creation
    std::string str();
    
    // Check if time is grater than this and reset if so. Return true if time has been reset.
    bool check( double time );
    
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
    
    size_t getSecondRunnig();
    
    int getSeconds();
    
};   


class ExecesiveTimeAlarm
{
    
    std::string title;
    double max_time;
    
    Cronometer c;
    
public:
    
    ExecesiveTimeAlarm( std::string _title )
    {
        title = _title;        
        max_time = 0.5;  // Default value
    }

    ExecesiveTimeAlarm( std::string _title , double _max_time )
    {
        title = _title;        
        max_time = _max_time;
    }
    
    ~ExecesiveTimeAlarm()
    {
        double t = c.diffTime();
        if(  t > max_time )
        {
            LM_T( LmtExcesiveTime , ("Excessive time ( %.4f > %.4f secs ) for '%s' "
                  ,t
                  ,max_time
                  ,title.c_str()
                  ));
        }
        
    }
    
};


class CronometerNotifier
{
    bool first_time;
    au::Cronometer cronometer;

    double initial_time;
    double period_time;
    
public:
    
    CronometerNotifier( double _period_time )
    {
        period_time = _period_time;
        first_time = false; // No initial time requirement
    }

    CronometerNotifier( double _initial_time ,  double _period_time )
    {
        initial_time = _initial_time;
        period_time = _period_time;
        first_time = true; // No initial time requirement
    }
    
    bool touch()
    {
        
        
        if ( first_time )
        {
            if ( cronometer.diffTime() > initial_time )
            {
                first_time = false;
                cronometer.reset();
                return true;
            }
            
            return false;
        }
        
        if ( cronometer.diffTime() > period_time )
        {
            cronometer.reset();
            return true;
        }
        return false;
    }
    
};

NAMESPACE_END

#endif
