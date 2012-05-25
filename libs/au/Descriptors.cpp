
#include "Descriptors.h"        // Own interface

NAMESPACE_BEGIN(au)

DescriptorsCounter::DescriptorsCounter( std::string _description)
{
    description = _description;
    counter = 0;
}

void DescriptorsCounter::increase()
{
    counter++;
}

std::string DescriptorsCounter::str()
{
    std::ostringstream output;
    output << counter << " X " << description; 
    return output.str();
}


Descriptors::~Descriptors()
{
    // Delete is called for each conunter
    concepts.clearMap();
}

size_t Descriptors::size()
{
    return concepts.size();
}


void Descriptors::add( std::string txt )
{
    DescriptorsCounter* counter = concepts.findInMap( txt );
    
    if( !counter )
    {
        counter = new DescriptorsCounter( txt );
        concepts.insertInMap( txt, counter );
    }
    
    counter->increase();
}

std::string Descriptors::str()
{
    au::map< std::string , DescriptorsCounter>::iterator i;
    
    std::ostringstream output;
    for ( i =  concepts.begin() ; i != concepts.end() ; i++)
        output << i->second->str() << " "; 
    return output.str();
}

NAMESPACE_END
