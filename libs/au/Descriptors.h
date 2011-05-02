
#ifndef _H_AU_DESCRIPTORS
#define _H_AU_DESCRIPTORS

#include <string>
#include <sstream>
#include "au/map.h"       // au::map

namespace au {

    
    class DescriptorsCounter
    {
        std::string description;
        int counter;
    public:
        DescriptorsCounter( std::string _description)
        {
            description = _description;
            counter = 0;
        }
        
        void increase()
        {
            counter++;
        }
        
        std::string str()
        {
            std::ostringstream output;
            output << counter << " X " << description; 
            return output.str();
        }
        
    };
    
    class Descriptors
    {
        
        au::map< std::string , DescriptorsCounter> concepts;
        
    public:
        
        ~Descriptors()
        {
            // Delete is called for each conunter
            concepts.clearMap();
        }
        
        void add( std::string txt )
        {
            DescriptorsCounter* counter = concepts.findInMap( txt );
            
            if( !counter )
            {
                counter = new DescriptorsCounter( txt );
                concepts.insertInMap( txt, counter );
            }
            
            counter->increase();
        }
        
        std::string str()
        {
            au::map< std::string , DescriptorsCounter>::iterator i;
            
            std::ostringstream output;
            for ( i =  concepts.begin() ; i != concepts.end() ; i++)
                output << i->second->str() << " "; 
            return output.str();
        }
        
    };

}

#endif