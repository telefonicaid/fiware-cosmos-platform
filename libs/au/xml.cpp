
#include "xml.h"        // Own interface

namespace au {
    
    void xml_open( std::ostringstream& output, std::string name )
    {
        output << "<" << name<<">";
    }
    
    void xml_close( std::ostringstream& output, std::string name )
    {
        output << "</" << name << ">";
    }
    
}
