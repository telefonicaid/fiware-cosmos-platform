
#include "time.h"         // au::Format
#include "au/string.h"      

#include "Cronometer.h"     // Own interface

NAMESPACE_BEGIN(au)

Cronometer::Cronometer()
{
    t = time( NULL );
}

void Cronometer::reset()
{
    t = time( NULL );
}

time_t Cronometer::diffTimeInSeconds()
{
    return time(NULL) - t;
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
