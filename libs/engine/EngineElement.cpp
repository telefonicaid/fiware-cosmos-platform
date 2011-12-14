
#include "au/xml.h"         // au::xml...


#include "engine/EngineElement.h"   // Own interface

NAMESPACE_BEGIN(engine)


EngineElement::EngineElement()
{
    delay = 0;
    repeated = false;
    counter = 0;

    gettimeofday(&trigger_time, NULL);
    trigger_time.tv_sec += delay;
    
    std::ostringstream txt;
    txt << "Unknown once ( at " << trigger_time.tv_sec  << " seconds ) engine element";
    description = txt.str();
    shortDescription = txt.str();
}

// Repeated action

EngineElement::EngineElement( int seconds )
{
    delay = seconds;
    repeated = true;
    counter = 0;

    gettimeofday(&trigger_time, NULL);
    trigger_time.tv_sec += delay;
    
    std::ostringstream txt;
    txt << "Unknown repeated ( at " << trigger_time.tv_sec << " every " << seconds  << " seconds ) engine element";
    description = txt.str();
    
    shortDescription = "Unk";
}

// Reschedule action once executed

void EngineElement::Reschedule()
{
    gettimeofday(&trigger_time, NULL);
    trigger_time.tv_sec += delay;
    counter++;
}

bool EngineElement::isRepeated()
{
    return repeated;
}

double EngineElement::getTimeToTrigger()
{
    timeval t;
    gettimeofday(&t, NULL);

    timeval t_diff;
    
    timersub( &trigger_time , &t , &t_diff);
    double time = t_diff.tv_sec + (t_diff.tv_usec / 1000000.0 );
    
    return time;
}

std::string EngineElement::getDescription()
{
    if( repeated )
    {
        return au::str( "[ %s repeated count:%d time:%02.2f delay:%d ] %s" 
                       , description.c_str()
                       , counter
                       , getTimeToTrigger()
                       , delay
                       , shortDescription.c_str() 
                       );
    }
    else
    {
        return au::str( "[ %s once time:%02.2f  ] %s" 
                       , description.c_str()
                       , getTimeToTrigger()
                       , shortDescription.c_str() 
                       );
    }
    //return description;
}

std::string EngineElement::getShortDescription()
{
    if( repeated )
    {
        return au::str( "[ EngineElement in %.2f secs ( repetition count:%d delay:%d ) ] %s" 
                       , getTimeToTrigger()
                       , counter
                       , delay
                       , shortDescription.c_str() 
                       );
    }
    else
    {
        return au::str( "[ EngineElement in %.2f secs ] %s" 
                       , getTimeToTrigger()
                       , counter
                       , delay
                       , shortDescription.c_str() 
                       );
        
    }
}

// get xml information
void EngineElement::getInfo( std::ostringstream& output)
{
    au::xml_open(output, "engine_element");
    
    au::xml_simple(output, "short_description", getShortDescription() );
    au::xml_simple(output, "description", getDescription() );
    
    au::xml_close(output, "engine_element");
}

NAMESPACE_END