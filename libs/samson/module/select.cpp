
#include "select.h" // Own interface


namespace samson
{
    void select::run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
    {
        LM_M(("Running select over %lu kvs (%s)" , inputs[0].num_kvs , environment->get( "command" ,  "no-command" ).c_str() ));
        
        for( size_t i = 0 ; i < inputs[0].num_kvs ; i++)
        {
            
            if( ( i%10000 ) == 0)
                operationController->reportProgress( (double) i / (double) inputs[0].num_kvs );
            
            //key.parse( inputs[0].kvs[i]->key );
            //value.parse( inputs[0].kvs[i]->value );
            //writer->emit( 0 , &value, &key);
        }
    }
    
}