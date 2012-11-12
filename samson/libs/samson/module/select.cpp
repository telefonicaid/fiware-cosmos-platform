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

#include "select.h" // Own interface

#include "au/CommandLine.h"         // au::CommandLine
#include "au/string.h"              // au::Format

/*
namespace samson
{
    
    
    void select::init(KVWriter *writer)
    {
        LM_M(("Init the select map: %d %d" , (int)inputData.size() , (int)outputData.size() ));
        
        au::CommandLine cmdLine;
        cmdLine.parse( environment->get("command" , "no_command" ) );

        std::string path_key    = cmdLine.get_argument(3);
        std::string path_value  = cmdLine.get_argument(4);

        std::string output_key_format; 
        std::string output_value_format; 
        
        
        key_data_instance = inputData[0].key;
        value_data_instance = inputData[0].value;
        
        // Get the data for the output key
        if( au::getRoot( path_key ) == "key" )
        {
            output_key_data_instance = key_data_instance;
            output_key_path = key_data_instance->getDataPath( au::getRest( path_key ) );
        }
        else if( au::getRoot( path_key ) == "value" )
        {
            output_key_data_instance = value_data_instance;
            output_key_path = value_data_instance->getDataPath( au::getRest( path_key ) );
        }
        else
        {
            LM_X(1,("Internal error"));
        }
        

        // Get the data for the output key
        if( au::getRoot( path_value ) == "key" )
        {
            output_value_data_instance = key_data_instance;
            output_value_path = key_data_instance->getDataPath( au::getRest( path_value ) );
        }
        else if( au::getRoot( path_value ) == "value" )
        {
            output_value_data_instance = value_data_instance;
            output_value_path = value_data_instance->getDataPath( au::getRest( path_value ) );
        }
        else
        {
            LM_X(1,("Internal error"));
        }
        
        
        LM_M(("Paths... %d %d" , output_key_path[0] , output_value_path[0] ));
        
        
        
        
    }
    
    void select::finish(KVWriter *writer)
    {
        
    }
    
    void select::run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
    {
        //LM_M(("Running select over %lu kvs (%s)" , inputs[0].num_kvs , environment->get( "command" ,  "no-command" ).c_str() ));
        
        for( size_t i = 0 ; i < inputs[0].num_kvs ; i++)
        {
            
            if( ( i%10000 ) == 0)
                operationController->reportProgress( (double) i / (double) inputs[0].num_kvs );

            key_data_instance->parse( inputs[0].kvs[i]->key );
            value_data_instance->parse( inputs[0].kvs[i]->value );
            
            //key.parse( inputs[0].kvs[i]->key );
            //value.parse( inputs[0].kvs[i]->value );
            //writer->emit( 0 , &value, &key);
        }
    }
    

}
 */
