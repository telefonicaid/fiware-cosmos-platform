
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
        return au::str( "[ Engine element '%s' to be executed in 02.2f seconds ( repeat every %d secs , repeated %d times )] " 
                       , description.c_str()
                       , getTimeToTrigger()
                       , delay
                       , counter
                       );
    }
    else
    {
        return au::str( "[ Engine element '%s' to be executed in %02.2f seconds]" 
                       , description.c_str()
                       , getTimeToTrigger()
                       );
    }
    //return description;
}


// get xml information
void EngineElement::getInfo( std::ostringstream& output)
{
    au::xml_open(output, "engine_element");
    au::xml_simple(output, "description", getDescription() );
    au::xml_close(output, "engine_element");
}

NAMESPACE_END