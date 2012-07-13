
#include "samson_system/ProcessComponent.h"
#include "samson_system/Value.h"
#include "samson_system/ValueVector.h"
#include "samson_system/ValueContainer.h"

#include "samson_system/TopProcessComponent.h"

#include "ProcessComponentManager.h" // Own interface

namespace samson{ namespace system{
    
    
    // ProcessComponentsManager
    
    ProcessComponentsManager::ProcessComponentsManager( )
    {
        // Setup components ( this will be based on a txt file in the future )
        add( new TopProcessComponent() );
    }
    
    void ProcessComponentsManager::add( ProcessComponent* component )
    {
        components_.push_back( component );
    }
    
    // Function used in process function
    void ProcessComponentsManager::process( samson::KVSetStruct* inputs , samson::KVWriter *writer )
    {
        
        ValueContainer keyContainer;
        ValueContainer stateContainer;
        
        ValueVector value_vector;
        
        // Recover key&state 
        // ---------------------------------------------------------
        if ( inputs[1].num_kvs > 0 )
        {
            keyContainer.value->parse( inputs[1].kvs[0]->key );
            stateContainer.value->parse( inputs[1].kvs[0]->value );
        }
        else if ( inputs[0].num_kvs > 0 )
        {
            keyContainer.value->parse( inputs[0].kvs[0]->key );
            stateContainer.value->set_as_void();
        }
        else
        {
            LM_E(("Error, nothing to do because no key"));
            return;  // Nothing to do if no key ( this should never happen )
        }
        
        // Recover values to be processed
        // ---------------------------------------------------------
        for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
            value_vector.add( inputs[0].kvs[i]->value );
        
        // Update state ( if possible )
        LM_M(("Calling ProcessComponentsManager::update"));
        update( keyContainer.value, stateContainer.value , value_vector.values_, value_vector.num_values_ , writer );
        
    }
    
    
    void ProcessComponentsManager::update( Value* key , Value * state , Value ** values , size_t num_values , samson::KVWriter *writer )
    {
        
        // Debug
        emit_log( "debug" , "----------------------------------------" , writer );
        emit_log( "debug" , au::str("Processing state %s - %s with %lu values" , key->str().c_str() , state->str().c_str() , num_values ) , writer );
        for( size_t i = 0 ; i < num_values ; i++ )
            emit_log( "debug" , au::str("Input value %lu/%lu: %s " , i , num_values , values[i]->str().c_str() ) , writer );
        
        // Look into components
        LM_M(("Loop for components, with components_.size():%lu", components_.size()));
        for ( size_t i = 0 , l = components_.size() ; i < l  ; i++ )
        {
            LM_M(("Calling component[%d](%s)->update()", i, components_[i]->name_.c_str()));
            if( components_[i]->update( key, state , values, num_values , writer ) )
            {
                LM_M(("Processed component[%d](%s)->update()", i, components_[i]->name_.c_str()));

                emit_log( "debug" , au::str("Processed this state with component %s" , components_[i]->name_.c_str() ) , writer );
                components_[i]->use_counter_++;
                
                // Check to swap positions in the vector
                while( ( i > 0 ) && ( components_[i]->use_counter_ > components_[i-1]->use_counter_ ) )
                {
                    LM_M(("Swapping components i(%d, '%s') with (i-1)(%d, '%s')", i, components_[i]->name_.c_str(), i-1, components_[i-1]->name_.c_str()));
                    // Swap positions
                    std::swap( components_[i] , components_[i-1] );
                    i--;
                }

                // Return since this key has been processed
                emit_log( "debug" , "----------------------------------------" , writer );
                return;
            }

            
        }
        
        emit_log( "debug" , "No component for this state"  , writer );
        emit_log( "debug" , "----------------------------------------" , writer );
        
        // Not processed since no component
        // Emit debug trace?
        
    }
    
}} // End of namespace samson.system        
