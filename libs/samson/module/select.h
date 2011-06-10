#ifndef _H_SAMSON_SELECT
#define _H_SAMSON_SELECT

#include "logMsg/logMsg.h"              // LM_M

#include "samson/module/KVFormat.h"     // samson::KVFormat
#include "samson/module/Operation.h"    // samson::Operation

#include "samson/module/Factory.h"

namespace samson
{
    
    // Spetial map operation to run selects
    
    class select : public samson::Map
    {
        
    public:
        
        void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer );        
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
#endif
