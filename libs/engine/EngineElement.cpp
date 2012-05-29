
#include "au/xml.h"         // au::xml...


#include "engine/EngineElement.h"   // Own interface

NAMESPACE_BEGIN(engine)


EngineElement::EngineElement( std::string name )
{
    // Keep tha name of this element
    name_ = name;
    
    // Flag to indicate that this element will be executed just once
    type = normal;
    
    description = "Engine element to be executed once";
    shortDescription = "Engine element to be executed once";
}

EngineElement::EngineElement( std::string name , int seconds )
{
    
    // Keep tha name of this element
    name_ = name;
    
    type = repeated;
    period = seconds;
    counter = 0;
    
    std::ostringstream txt;
    txt << "Engine element repeated every " << seconds  << " seconds";
    description = txt.str();
    shortDescription = txt.str();
}

// Reschedule action once executed

void EngineElement::Reschedule()
{
    // Reset cronometer
    cronometer.reset();

    // Increse the counter to get an idea of the number of times a repeated task is executed
    counter++;
}


double EngineElement::getTimeToTrigger()
{
    // Time for the next execution
    if( type == repeated )
    {
        double t = cronometer.diffTime();
        LM_T(LmtEngine, ("getTimeToTrigger: Period %d Cronometer: %f" , period , t ));
        return period - t; 
    }
    return 0;
}

double EngineElement::getPeriod()
{
    return period;
}

double EngineElement::getWaitingTime()
{
    // Time for the next execution
    return cronometer.diffTime(); 
}


std::string EngineElement::getDescription()
{
    return description;
}

std::string EngineElement::str()
{
    if( type == repeated )
    {
        return au::str( "%s [ Engine element to be executed in %02.2f seconds ( repeat every %d secs , repeated %d times )] "
                       , description.c_str()
                       , getTimeToTrigger()
                       , period
                       , counter
                       );
    }
    else if( type == extra )
        return au::str( "%s [ Engine element EXTRA ]" , description.c_str() );
    else
        return au::str( "%s [ Engine element ]" , description.c_str() );
}


// get xml information
void EngineElement::getInfo( std::ostringstream& output)
{
    au::xml_open(output, "engine_element");
    au::xml_simple(output, "description", str() );
    au::xml_close(output, "engine_element");
}

std::string EngineElement::getName()
{
    return name_;
}


bool EngineElement::isRepeated()
{
    return (type==repeated);
}

bool EngineElement::isExtra()
{
    return (type==extra);
}

bool EngineElement::isNormal()
{
    return (type==normal);
}




NAMESPACE_END
