
#include "samson/common/EnvironmentOperations.h"			// Own definition of this methods

#include <iostream>
#include <iomanip>


namespace samson {

	void copyEnviroment( Environment* from , network::Environment * to )
	{
		std::map<std::string,std::string>::iterator iter;
		for ( iter = from->environment.begin() ; iter != from->environment.end() ; iter++)
		{
			network::EnvironmentVariable *ev = to->add_variable();			
			
			ev->set_name( iter->first );
			ev->set_value( iter->second );
		}
		
	}

	void copyEnviroment( const network::Environment & from , Environment* to  )
	{
		for( int i = 0 ; i < from.variable_size() ; i++ )
		{
			std::string name = from.variable(i).name();
			std::string value = from.variable(i).value();
			to->set( name , value );
		}
	}
    
    void copy( KVInfo * from , network::KVInfo* to)
    {
        to->set_kvs(from->kvs);
        to->set_size(from->size);
    }
    
    void copy( FullKVInfo * from , network::KVInfo* to)
    {
        to->set_kvs(from->kvs);
        to->set_size(from->size);
    }
    
    
    std::string getStatus( network::StreamQueue *queue )
    {
        std::ostringstream txt;
        
        txt << std::setw(30) << queue->name();
        txt << " (" << queue->format().keyformat() << " " << queue->format().valueformat() << ") ";
        txt << " --> " << queue->operation() << " --> ";
        
        for (int j = 0 ;  j < queue->output_size() ; j++)
        {
            txt << " [ ";
            for (int k = 0 ; k < queue->output(j).queue_size() ; k++ )
            {
                txt << queue->output(j).queue(k);
                if( k != (queue->output(j).queue_size()-1) )
                    txt << ",";
            }
            txt << " ] ";
        }
        
        
        return txt.str();
    }
    
}