
/* ****************************************************************************
 *
 * FILE                     Cronometer.cpp - Cronometer to meassure the ellapsed time
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            April 2011
 *
 */


#include "Cronometer.h"     // Own interface
#include "Format.h"         // au::Format

namespace au {

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
        return Format::time_string(diffTimeInSeconds());
    }


}