
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_system_map_value_to_3_value
#define _H_SAMSON_system_map_value_to_3_value


#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>


namespace samson{
namespace system{


	class map_value_to_3_value : public samson::Map
	{

        std::string command;
        
        samson::system::Value key;
        samson::system::Value value;
        
        // Collection of filters to execute for ecery key-value
        FilterCollection filters_collection;

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.Value system.Value  
//  output: system.Value system.Value
//  output: system.Value system.Value
//  
// helpLine: Generic map to transform values. User enviroment variable command to specify what to do...
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
            // Setup the process chain...
            command =  environment->get( "command" ,  "" );
            au::ErrorManager error;
            filters_collection.addFilters( command , writer , &error );
            
            // Error defined by user
            if( error.isActivated() )
                tracer->setUserError( error.getMessage() );

		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
            for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
            {
                key.parse( inputs[0].kvs[i]->key );
                value.parse( inputs[0].kvs[i]->value );
                
                // ---------------------------------------------------------
                // Run the filters
                // ---------------------------------------------------------
                
                KeyValue kv( &key, &value );
                for( size_t f = 0 ; f < filters_collection.filters.size() ; f++ )
                    filters_collection.filters[f]->run(kv);
            }
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace system
} // end of namespace samson

#endif
