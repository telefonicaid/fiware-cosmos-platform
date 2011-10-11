


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

#include "au/string.h"      // au::tabs
#include "au/Tree.h"        // au::TreeItem

#include "pugi.h"			// Own interface

namespace pugi
{
    
    std::string str( const xml_document& xml_document , int deep )
    {
        std::ostringstream output;
        
        str( xml_document , 0 , output , deep );
        
        return output.str();
    }
    
    void str( const xml_node& xml_node , int level , std::ostringstream &output , int deep )
    {
        if( deep <= 0)
            return;
        
        std::string name =  xml_node.name();
        std::string value =  xml_node.value();
        
        output << au::tabs(level) << name  << " - " << value;
        
        int num_attributes = 0;
        std::ostringstream attributes_txt;
        {
            // Print attributes
            attributes_txt << " ( ";
            for (pugi::xml_attribute_iterator ait = xml_node.attributes_begin(); ait != xml_node.attributes_end(); ++ait)
            {
                attributes_txt << ait->name() << "=" << ait->value() << " ";
                num_attributes++;
            }
            attributes_txt << " " << num_attributes << " attributes";
            
            attributes_txt << ")";
        }
        if( num_attributes > 0 )
            output << attributes_txt.str();
        
        output << "\n";
        
        // Childrens
        for( pugi::xml_node_iterator n = xml_node.begin() ; n != xml_node.end() ; n++)
            str( *n , level+1 , output , deep - 1 );
    }
    
    void str( const xpath_node_set & xpath_node_set , std::ostringstream &output , int deep )
    {
        for( size_t i = 0 ; i < xpath_node_set.size() ; i++ )
            str( xpath_node_set[i] , output , deep - 1 );
    }
    
    void str( const xpath_node& xpath_node , std::ostringstream &output , int deep )
    {
        if( deep <= 0)
            return;
        
        // Check if this is an attribute
        std::string attr_value = xpath_node.attribute().value();
        
        if( attr_value != ""  )
            output << "Attribute " << xpath_node.attribute().name() << ": " << xpath_node.attribute().value() << "\n";
        else
            str( xpath_node.node() , 0 , output , deep - 1 );
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
    
    
    // Getting nodes from a query
    
    xpath_node_set select_nodes( const xml_node& _node , std::string query )
    {
        pugi::xpath_node_set result;
        
        try {
            result = _node.select_nodes( query.c_str() );
            
        } catch (pugi::xpath_exception& ex) 
        {
        }
        return result;
        
    }    

    double getDouble( const xml_node& node , std::string child_name )
    {
        return strtod(  node.first_element_by_path( child_name.c_str()  ).child_value()  , (char **)NULL );
    }
    
    size_t getUInt64( const xml_node& node , std::string child_name )
    {
        return strtoll(  node.first_element_by_path( child_name.c_str()  ).child_value()  , (char **)NULL, 10);
    }

    size_t getInt( const xml_node& node , std::string child_name )
    {
        return (int)strtol(  node.first_element_by_path( child_name.c_str()  ).child_value()  , (char **)NULL, 10);
    }
    
    std::string get( const xml_node& node , std::string child_name )
    {
        return node.first_element_by_path( child_name.c_str() ).child_value();
        
    }
    
    int getNumChildrens( const xml_node& xml_node , std::string child_name )
    {
        int num = 0;
        for( pugi::xml_node_iterator n = xml_node.begin() ; n != xml_node.end() ; n++)
            if( n->name() == child_name )
                num++;
        return num;
    }

    int getNumChildrens( const xml_node& xml_node )
    {
        int num = 0;
        for( pugi::xml_node_iterator n = xml_node.begin() ; n != xml_node.end() ; n++)
                num++;
        return num;
    }

    xml_node getFirstChildren( const xml_node& xml_node )
    {
        return *xml_node.begin();
    }
    
    
    au::DataSetRow* rowFromNode( const xml_node& xml_node , int limit )
    {
        au::DataSetRow * row = new au::DataSetRow();
        

        for( pugi::xml_node_iterator n = xml_node.begin() ; n != xml_node.end() ; n++)
        {
            pugi::xml_node new_node = *n;
            addToRow(row, new_node, "", limit);
        }
        
        return row;
    }
    
    void addToRow( au::DataSetRow* row , const xml_node& xml_node , std::string path , int limit )
    {
        if( limit <= 0)
            return;
        
        std::string new_path = path + "/" + xml_node.name();;
            
        if( ( getNumChildrens( xml_node ) == 1) &&  ( getNumChildrens( getFirstChildren( xml_node) ) == 0 ) )
        {
            // Set a property
            std::string field = new_path;
            std::string value = getFirstChildren( xml_node).value();

            row->set(field, value);
        }
        
        for( pugi::xml_node_iterator n = xml_node.begin() ; n != xml_node.end() ; n++)
        {
            pugi::xml_node new_node = *n;
            addToRow(row, new_node, new_path, limit-1);
        }
    }
    
    void dataSetFromNodes( au::DataSet &dataSet , xpath_node_set nodes )
    {
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
        {
            const pugi::xml_node& node = nodes[i].node(); 
            dataSet.add( pugi::rowFromNode( node , 3 ) );
        }      
    }    
    
    au::TreeItem* treeItemFromDocument(  const xml_document& xml_doc )
    {
        return treeItemFromNode( xml_doc.document_element() );
    }    
    au::TreeItem* treeItemFromNode(  const xml_node& xml_node )
    {
        std::string name = xml_node.name();
        if( name.length() == 0)
            name = xml_node.value();
        
        au::TreeItem* treeItem = new au::TreeItem( name );
        
        for( pugi::xml_node_iterator n = xml_node.begin() ; n != xml_node.end() ; n++)
        {
            pugi::xml_node node = *n;
            treeItem->add( treeItemFromNode(node) );
        }      
        
        return treeItem;
    }
    
/*   
    au::TreeItem* treeItemFromNodes( xpath_node_set nodes )
    {
        au::TreeItem* treeItem = new au::TreeItem("root");
        
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
        {
            const pugi::xml_node& node = nodes[i].node(); 
            treeItem->add( treeItemFromNode(node) );
        }      
        
        return treeItem;
        
    }
 */
    
    
}
