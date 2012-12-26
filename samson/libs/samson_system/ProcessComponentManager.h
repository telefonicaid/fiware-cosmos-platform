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

#ifndef _H_SAMSON_system_PROCESS_COMPOENT_MANAGER
#define _H_SAMSON_system_PROCESS_COMPOENT_MANAGER


#include "au/containers/map.h"
#include <samson/module/samson.h>
#include <samson_system/Value.h>
#include "samson/module/KVWriter.h"

namespace samson{ namespace system{

    class ProcessComponent;
    
    /*
     
     Manager of ProcessComponent's
     Used inside system.process operation to update states based on input values
     
     */
    
    class ProcessComponentsManager
    {

        // Vecor of components to be used to process this
        au::vector<ProcessComponent> components_;
        
    public:

        ProcessComponentsManager( );

        // Add new process components
        void add( ProcessComponent* component );
        
        // Function used in process function
        void process( samson::KVSetStruct* inputs , samson::KVWriter *writer );
        
    private:
        
        void update( Value* key , Value * state , Value ** values , size_t num_values , samson::KVWriter *writer );
        
        
    };

    
}} // End of namespace samson.system        

#endif

