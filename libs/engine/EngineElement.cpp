
#include "au/xml.h"         // au::xml...


#include "engine/EngineElement.h"   // Own interface

NAMESPACE_BEGIN(engine)


EngineElement::EngineElement()
{
    delay = 0;
    repeated = false;
    thiggerTime = time(NULL) + delay;
    counter = 0;
    
    
    std::ostringstream txt;
    txt << "Unknown once ( at " << thiggerTime  << " seconds ) engine element";
    description = txt.str();
    shortDescription = txt.str();
}

// Repeated action

EngineElement::EngineElement( int seconds )
{
    delay = seconds;
    repeated = true;
    thiggerTime = time(NULL) + delay;
    counter = 0;
    
    std::ostringstream txt;
    txt << "Unknown repeated ( at " << thiggerTime << " every " << seconds  << " seconds ) engine element";
    description = txt.str();
    
    shortDescription = "Unk";
}

// Reschedule action once executed

void EngineElement::Reschedule()
{
    thiggerTime += delay;
    counter++;
}

void EngineElement::Reschedule(time_t now)
{
    thiggerTime = now + delay;
    counter++;
}

time_t EngineElement::getThriggerTime()
{
    return thiggerTime;
}

bool EngineElement::isRepeated()
{
    return repeated;
}

std::string EngineElement::getDescription()
{
    if( repeated )
        return au::str( "[%s repeated count:%d triggerTime:%lu delay:%d ] %s" , description.c_str(), counter, thiggerTime, delay , shortDescription.c_str() );
    else
        return au::str( "[%s once triggerTime:%lu  ] %s" , description.c_str(), thiggerTime , shortDescription.c_str() );
    //return description;
}

std::string EngineElement::getShortDescription()
{
    if( repeated )
        return au::str( "[Repetition element count:%d triggerTime:%lu delay:%d ] %s" , counter, thiggerTime, delay , shortDescription.c_str() );
    else
        return shortDescription;
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