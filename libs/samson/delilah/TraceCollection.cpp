

#include "TraceCollection.h" // Own interface


namespace samson {

    TraceCollection::TraceCollection()
    {
        max_num_elements = 100;
    }
    
    void TraceCollection::add( NodeIdentifier node , std::string type , std::string context , std::string text )
    {
        traces.push_back( new Trace( node , type , context , text ) );
        
        while( traces.size() > max_num_elements )
        {
            Trace *trace = traces.front();
            delete trace;
            traces.pop_front();
        }
    }
    
    std::string TraceCollection::str()
    {
        au::tables::Table table( 
                                au::StringVector( "From" , "Type" , "Context" , "Message" ),
                                au::StringVector( "left" , "" , "" , "left" )
                                );
        
        au::list<Trace>::iterator it_traces;
        for (it_traces = traces.begin() ; it_traces != traces.end() ; it_traces ++ )
        {
            Trace * trace = (*it_traces);
            au::StringVector values;
            values.push_back( trace->node.str() );
            values.push_back( trace->type );
            values.push_back( trace->context );
            values.push_back( trace->text );
            
            table.addRow( values );
        }
        
        
        return table.str("Traces");
        
        
        
    }
}