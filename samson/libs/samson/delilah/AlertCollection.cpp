/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


#include "AlertCollection.h" // Own interface


namespace samson {

    AlertCollection::AlertCollection()
    {
        max_num_elements = 100;
    }
    
    void AlertCollection::add( NodeIdentifier node , std::string type , std::string context , std::string text )
    {
        traces.push_back( new Alert( node , type , context , text ) );
        
        while( traces.size() > max_num_elements )
        {
            Alert *trace = traces.front();
            delete trace;
            traces.pop_front();
        }
    }
    
    std::string AlertCollection::str()
    {
        au::tables::Table table( "From,left|Type|Context|Message,left" );
        
        au::list<Alert>::iterator it_traces;
        for (it_traces = traces.begin() ; it_traces != traces.end() ; it_traces ++ )
        {
            Alert * trace = (*it_traces);
            au::StringVector values;
            values.push_back( trace->node.str() );
            values.push_back( trace->type );
            values.push_back( trace->context );
            values.push_back( trace->text );
            
            table.addRow( values );
        }
        
        table.setTitle("Alerts");
        return table.str();
        
        
        
    }
}