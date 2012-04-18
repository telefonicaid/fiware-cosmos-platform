

#include "samson_system/Value.h" // Own interface

namespace samson 
{
    namespace system 
    {
        
        
        // Static pool for object resusage
        au::Pool<Value> samson::system::Value::pool_values;

        
        // --------------------------------------------------------
        // Constant word serialization stuff
        // --------------------------------------------------------
        
        // Constant sorted alphabetically for fast search
        const char* constant_words[] =
        {
            "log"
            "url",
            "user",
            "usr",
        };

        int internal_get_constant_word_code( const char * word , int min , int max )
        {
            if( max <= (min+1) )
                return -1;
            
            int mid = (min + max)/2;
            
            int c = strcmp(word , constant_words[mid] );
            if(  c == 0 )
                return mid;
            else if( c > 0 )
                return internal_get_constant_word_code(word, mid, max);
            else
                return internal_get_constant_word_code(word, min, mid);
        }
        
        // Find if this word is a constant word ( serialized with 2 bytes )
        // Return -1 if not found
        int get_constant_word_code( const char * word )
        {
            int max = sizeof(constant_words)/sizeof(char*);
            
            if( strcmp(word , constant_words[0] ) == 0 )
                return 0;
            if( strcmp(word , constant_words[max-1] ) == 0 )
                return max-1;
            return internal_get_constant_word_code(word, 0,max-1);
        }

        const char* get_constant_word( int c )
        {
            if( ( c < 0 ) || (c > 255))
                return "Unknown";
            return constant_words[c];
        }

     
        void Value::setFromXmlString( const char * data , au::ErrorManager *error )
        {
		   
            // document to parse xml
            pugi::xml_document xml_doc;
            
            // Parser the xml document into "doc"
            pugi::xml_parse_result result = xml_doc.load( data );
            
            // Check errors in the parsing
            if( result.status != pugi::status_ok )
            {
                error->set( result.description() );
                set_as_void();
                return;
            }

            // Navigate the xml tree creating a map element
            setFromXmlNode( xml_doc );
        }
        
        void Value::setFromXmlNode(  const pugi::xml_node& xml_node )
        {
            
            switch (xml_node.type()) 
            {
                case pugi::node_null:
                {
                    set_as_map();
                    add_value_to_map("type")->set_string("void");
                    return;
                }
                    
                case pugi::node_document:
                {
                    // Main document... just skip to main element
                    pugi::xml_node_iterator n = xml_node.begin();
                    if( n!= xml_node.end() )
                        setFromXmlNode( *n );
                    break;
                }
                    
                case pugi::node_element:    // Element tag, i.e. '<node/>'
                {
                    set_as_map();
                    add_value_to_map("type")->set_string("node");
                    add_value_to_map("name")->set_string( xml_node.name() );

                    Value * childrens = add_value_to_map("childrens");
                    childrens->set_as_vector();
                    for( pugi::xml_node_iterator n = xml_node.begin() ; n != xml_node.end() ; n++)
                    {
                        // For each node
                        pugi::xml_node node = *n;
                        
                        childrens->add_value_to_vector()->setFromXmlNode(*n);
                    }      
                }
                    break;
                    
                case pugi::node_pcdata:		    // Plain character data, i.e. 'text'
                case pugi::node_cdata:			// Character data, i.e. '<![CDATA[text]]>'
                {
                    set_as_map();
                    add_value_to_map("type")->set_string("value");
                    add_value_to_map("value")->set_string( xml_node.value() );
                    break;
                }

                case pugi::node_comment:		 // Comment tag, i.e. '<!-- text -->'
                case pugi::node_pi:			     // Processing instruction, i.e. '<?name?>'
                case pugi::node_declaration :	 // Document declaration, i.e. '<?xml version="1.0"?>'
                case pugi::node_doctype :        // Document type declaration, i.e. '<!DOCTYPE doc>'

                    set_as_map();
                    add_value_to_map("type")->set_string("unknown");
                    
                    break;
                    
            }
            
        }
        
    }
}
