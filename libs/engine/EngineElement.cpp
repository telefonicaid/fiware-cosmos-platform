

#include "engine/EngineElement.h"   // Own interface

namespace engine {
    
    
    EngineElement::EngineElement()
    {
        delay = 0;
        repeated = false;
        thiggerTime = time(NULL) + delay;
        counter = 0;
        
        description = "Unkown non-repeated engine element";
        shortDescription = "Unk";
    }
    
    // Repeated action
    
    EngineElement::EngineElement( int seconds )
    {
        delay = seconds;
        repeated = true;
        thiggerTime = time(NULL) + delay;
        counter = 0;
        
        std::ostringstream txt;
        txt << "Unkown repeated ( every " << seconds  << " seconds ) engine element";
        description = txt.str();
        
        shortDescription = "Unk";
    }
    
    // Reschedule action once executed
    
    void EngineElement::Reschedule()
    {
        thiggerTime += delay;
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
        return description;
    }
    
    std::string EngineElement::getShortDescription()
    {
        if( repeated )
            return au::str( "[Repetition element period:%d count:%d ] %s" , counter, delay , shortDescription.c_str() );
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
    
    
}