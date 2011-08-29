

/* ****************************************************************************
 *
 * FILE            Pugi.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         pugi
 *
 * DATE            7/15/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_xml_Pugi
#define _H_xml_Pugi


#include <string>           // std::string
#include <sstream>          // std::ostringstream
#include <vector>
#include <set>

#include "pugi/pugixml.hpp"     // pugi::...

#include "au/Format.h"          // au::F

namespace pugi 
{
    
    typedef const xml_node& node;
    
    class ValuesCollection : public std::vector<std::string>
    {
        
    public:
        
        ValuesCollection()
        {
            //Empty set
        }
        
        ValuesCollection( const xpath_node_set & xpath_node_set )
        {
            // Collect the results from the this "select"
            
            for( size_t i = 0 ; i < xpath_node_set.size() ; i++ )
                push_back( getValue( xpath_node_set[i] ) );
            
        }
        
        
        std::string getValue( const xpath_node & xpath_node )
        {
            // Check if this is an attribute
            std::string attr_value = xpath_node.attribute().value();
            
            
            if( attr_value != ""  )
                return attr_value;
            else
                return xpath_node.node().child_value();
        }
        
        
        std::string str( )
        {
            std::ostringstream output;
            output << "[ "; 

            for ( size_t i = 0 ; i < size() ; i++ )
                output << (*this)[i] << " ";
            output << "]"; 
            return output.str();
        }
        
        ValuesCollection uniq()
        {

            // Get a set of unique values
            std::set< std::string > unique_values;
            for ( size_t i = 0 ; i < size() ; i++ )
                unique_values.insert( (*this)[i] );

            // Full a new ValuesCollection instance and return it
            ValuesCollection tmp;
            std::set< std::string >::iterator v;
            for( v = unique_values.begin() ; v != unique_values.end() ; v++)
                tmp.push_back( *v );
            
            return tmp;
        }
        
        
    };
    
    // Main function to select a set of nodes with an xpath query
    xpath_node_set select_nodes( const xml_node& _node , std::string query );
        
    
    // Function to transform pugi elements to string
    std::string str( const xml_document& xml_document , int deep );
    void str( const xml_node& xml_node , int level , std::ostringstream &output , int deep );
    void str( const xpath_node_set & xpath_node_set , std::ostringstream &output , int deep );
    void str( const xpath_node& xpath_node , std::ostringstream &output , int deep );
    
    // Run a query to get values , uint64 or string
    ValuesCollection values( const xml_document & doc , std::string query );
    size_t UInt64( const xml_document & doc , std::string query_str );
    std::string String( const xml_document & doc , std::string query_str );
    double Double( const xml_document & doc , std::string query_str );
 
    
    // Function to extract information form xml_node in an easy way
    
    size_t getUInt64( const xml_node& node , std::string child_name );
    size_t getInt( const xml_node& node , std::string child_name );
    std::string get( const xml_node& node , std::string child_name );
}
#endif
