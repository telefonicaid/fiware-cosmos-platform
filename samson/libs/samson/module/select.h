#ifndef _H_SAMSON_SELECT
#define _H_SAMSON_SELECT

#include "logMsg/logMsg.h"              // LM_M

#include "samson/module/KVFormat.h"     // samson::KVFormat
#include "samson/module/Operation.h"    // samson::Operation

#include "samson/module/Factory.h"
/*

namespace samson
{
    
    // Spetial map operation to run selects
    
    class select : public samson::Map
    {
        DataInstance* key_data_instance;
        DataInstance* value_data_instance;
        
        // How to obtain the output key
        int* output_key_path;
        DataInstance *output_key_data_instance;

        // How to obtain the output value
        int* output_value_path;
        DataInstance *output_value_data_instance;
        
    public:
        
		void init(KVWriter *writer);
        
	    void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer );       
        
		void finish(KVWriter *writer);
        
    };
    
    
    // Select Operation
    class SelectOperation : public samson::Operation
    {
        
        std::string command;
        
    public:
        
        SelectOperation( ) : samson::Operation( "select" , samson::Operation::map , au::factory<select> )
        {
            inputFormats.push_back( KVFormat( "*" , "*" ) );
            outputFormats.push_back( KVFormat( "*" , "*" ) );
        }
        
    };
    
    
}
 
 */


#endif
