
#include "time.h"         // au::Format
#include "au/string.h"      

#include "Cronometer.h"     // Own interface

NAMESPACE_BEGIN(au)

Cronometer::Cronometer()
{
    if ( gettimeofday(&t,NULL) != 0)
        LM_X(1, ("gettimeofday failed"));
}

void Cronometer::reset()
{
    if ( gettimeofday(&t,NULL) != 0)
        LM_X(1, ("gettimeofday failed"));
}

time_t Cronometer::diffTimeInSeconds()
{
    struct timeval t2;
    
    if ( gettimeofday(&t2,NULL) != 0)
        LM_X(1, ("gettimeofday failed"));

    return t2.tv_sec - t.tv_sec;
}

double Cronometer::diffTimeAndReset()
{
    struct timeval t2;
    
    if ( gettimeofday(&t2,NULL) != 0)
        LM_X(1, ("gettimeofday failed"));

    // Reset the value to this new time reference
    t = t2;
    
    return ( (double) t2.tv_sec ) + ((double)t2.tv_usec / 1000000.0) - ( (double) t.tv_sec ) - ((double)t.tv_usec / 1000000.0);
    
}

double Cronometer::diffTime()
{
    struct timeval t2;
    
    if ( gettimeofday(&t2,NULL) != 0)
        LM_X(1, ("gettimeofday failed"));
    
    return ( (double) t2.tv_sec ) + ((double)t2.tv_usec / 1000000.0) - ( (double) t.tv_sec ) - ((double)t.tv_usec / 1000000.0);

}

std::string Cronometer::str()
{
    return time_string(diffTimeInSeconds());
}

#pragma mark CronometerSystem    

CronometerSystem::CronometerSystem()
{
    running = false;
    seconds = 0;
}

void CronometerSystem::start()
{
    running = true;
    c.reset();
}
void CronometerSystem::stop()
{
    running = false;
    seconds += c.diffTimeInSeconds();
}

// Reset the count to 0
void CronometerSystem::reset()
{
    running = false;
    seconds = 0;
}

// Get a string with the time ellapsed since last reset or creation
std::string CronometerSystem::str()
{
    return time_string( getSeconds() );
}

int CronometerSystem::getSeconds()
{
    if( running )
        return (seconds + c.diffTimeInSeconds() );
    else
        return seconds;
}

NAMESPACE_END
