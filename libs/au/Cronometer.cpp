
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

    double time_dif = ( (double) t2.tv_sec ) + ((double)t2.tv_usec / 1000000.0) - ( (double) t.tv_sec ) - ((double)t.tv_usec / 1000000.0);
    
    // Reset the value to this new time reference
    t = t2;
    
    return time_dif;
    
}

double Cronometer::diffTime()
{
    struct timeval t2;
    
    if ( gettimeofday(&t2,NULL) != 0 )
        LM_X(1, ("gettimeofday failed"));
    
    double diff = ( (double) t2.tv_sec ) + ((double)t2.tv_usec / (double)1000000.0 ) 
    - ( (double) t.tv_sec ) - ((double)t.tv_usec / (double)1000000.0);

/*    
    {
        LM_M(("Cronometer diff %.02f (%d %d --> %d %d )"
              ,diff
              ,t.tv_sec
              ,t.tv_usec
              ,t2.tv_sec
              ,t2.tv_usec
              ));
    }
*/    
    return diff;
}

bool Cronometer::check( double time )
{
    if( diffTime() >= time )
    {
        reset();
        return true;
    }
    return false;
}


std::string Cronometer::str()
{
   return str_time( diffTimeInSeconds() );
}

std::string Cronometer::str_simple()
{
    return str_time_simple( diffTimeInSeconds() );
}


std::string Cronometer::strClock()
{
    char c = '-';
    switch ( (int) ( diffTime() * 3 ) % 4 ) 
    {
        case 0:
            c = '-';
            break;
        case 1:
            c = '\\';
            break;
        case 2:
            c = '|';
            break;
        default:
            c = '/';
            break;
    }
    return au::str("%c %s" , c , str_time(diffTimeInSeconds()).c_str() );
    
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
   return str_time( (size_t) getSeconds() );
}

size_t CronometerSystem::getSecondRunnig()
{
    if( running )
        return seconds + c.diffTime();
    else
        return seconds;
}


int CronometerSystem::getSeconds()
{
    if( running )
        return (seconds + c.diffTimeInSeconds() );
    else
        return seconds;
}

NAMESPACE_END
