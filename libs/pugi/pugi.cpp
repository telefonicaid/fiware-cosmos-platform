


/* ****************************************************************************
 *
 * FILE            pugi.cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         pugi
 *
 * DATE            7/15/11
 *
 * DESCRIPTION
 *
 *  Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#include "pugi.h"			// Own interface


namespace pugi
{
    
    std::string str( const xml_document& xml_document )
    {
        std::ostringstream output;
        
        str( xml_document , 0 , output );
        
        return output.str();
    }
    
    void str( const xml_node& xml_node , int level , std::ostringstream &output )
    {
        output << au::F::tabs(level) << xml_node.name() << " ( " << xml_node.value() << " ) ";
        
        // Print attributes
        output << " ( ";
        for (pugi::xml_attribute_iterator ait = xml_node.attributes_begin(); ait != xml_node.attributes_end(); ++ait)
            output << ait->name() << "=" << ait->value() << " ";
        output << ") ";
        
        output << " [" << xml_node.child_value() << "]\n";
        
        // Childrens
        for( pugi::xml_node_iterator n = xml_node.begin() ; n != xml_node.end() ; n++)
            str( *n , level+1 , output );
        
    }
    
    
    void str( const xpath_node_set & xpath_node_set , std::ostringstream &output )
    {
        for( size_t i = 0 ; i < xpath_node_set.size() ; i++ )
            str( xpath_node_set[i] , output );
    }
    
    void str( const xpath_node& xpath_node , std::ostringstream &output )
    {
        // Check if this is an attribute
        std::string attr_value = xpath_node.attribute().value();
        
        if( attr_value != ""  )
            output << "Attribute " << xpath_node.attribute().name() << ": " << xpath_node.attribute().value() << "\n";
        else
            str( xpath_node.node() , 0 , output );
    }
    
    
    
    // Run a query to get values
    
    ValuesCollection values( const xml_document & doc , std::string query )
    {
        pugi::xpath_node_set result;
        
        try {
            result = doc.select_nodes( query.c_str() );
        } catch (pugi::xpath_exception& ex) 
        {
            return ValuesCollection();
        }
        
        // Normal process of the values
        ValuesCollection vc( result );
        
        return vc;
    }
    
    
    size_t UInt64( const xml_document & doc , std::string query_str )
    {
        
        pugi::xpath_query query( query_str.c_str()  );                    
        double result = query.evaluate_number( doc );
        
        if( query.result() )
            return (size_t) result;
        else
            return 0;
        
    }

    double Double( const xml_document & doc , std::string query_str )
    {
        
        pugi::xpath_query query( query_str.c_str()  );                    
        double result = query.evaluate_number( doc );
        
        if( query.result() )
            return result;
        else
            return 0;
        
    }
    
    
    std::string String( const xml_document & doc , std::string query_str )
    {
        pugi::xpath_query query( query_str.c_str()  );                    
        return query.evaluate_string( doc );
    }
    
    
}
