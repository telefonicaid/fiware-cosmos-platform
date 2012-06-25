
#ifndef _H_SAMSON_system_PROCESS_COMPONENT
#define _H_SAMSON_system_PROCESS_COMPONENT

#include "samson/module/KVWriter.h"
#include "samson_system/Value.h"

namespace samson{ namespace system{
    
    class ProcessComponent
    {
        std::string name_;  // Debug name
        int use_counter_;   // Internal use-counter to reorder ProcessComponent's inside a ProcessComponentsManager
        
        friend class ProcessComponentsManager;
        
    public:
        
        ProcessComponent( std::string name )
        {
            name_ = name;
            use_counter_ = 0; // Init counter
        }
        
        // Update this state based on input values ( return true if this state has been updated with this component )
        virtual bool update( Value* key , 
                            Value * state , 
                            Value ** values , 
                            size_t num_values , 
                            samson::KVWriter *writer )=0;
    };
    
    
    // Handy functions ( MACROs ? )
    void emit_output(  Value* key , Value * state , samson::KVWriter * writer );
    void emit_feedback(  Value* key , Value * state , samson::KVWriter * writer );
    void emit_state(  Value* key , Value * state , samson::KVWriter * writer );
    void emit_log( const std::string& key , const std::string& message , samson::KVWriter *writer  );
    
    
    
} } // End of namespace

#endif