
#include "xml.h"        // Own interface

NAMESPACE_BEGIN(au)

void xml_open( std::ostringstream& output, std::string name )
{
    output << "<" << name<<">";
}

void xml_close( std::ostringstream& output, std::string name )
{
    output << "</" << name << ">";
}

void xml_content( std::ostringstream& output , std::string _content )
{
    output << "<![CDATA[" << _content << "]]>";        
}

NAMESPACE_END