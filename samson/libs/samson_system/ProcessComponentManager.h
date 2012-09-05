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

/*
 * FILE            ProcessComponentsManager.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Declaration of ProcessComponentsManager class to prepare data and call
 *  the Update method implemented in the different child classes
 *
 */

#ifndef _H_SAMSON_system_PROCESS_COMPONENT_MANAGER
#define _H_SAMSON_system_PROCESS_COMPONENT_MANAGER

#include "au/containers/vector.h"
#include "samson/module/KVWriter.h"
#include "samson/module/KVSetStruct.h"
#include "samson/module/samson.h"
#include "samson_system/ProcessComponent.h"
#include "samson_system/Value.h"

namespace samson {
namespace system {
class ProcessComponent;

/*
 *
 * Manager of ProcessComponent's
 * Used inside system.process operation to update states based on input values
 *
 */

class ProcessComponentsManager {

  public:

    ProcessComponentsManager();

    // Add new process components
    void Add(ProcessComponent* const component);

    // Function used in process function
    void Process(samson::KVSetStruct *inputs, samson::KVWriter* const writer);

  private:
    void Update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);

    // Vector of components to be used to process this
     au::vector<ProcessComponent> components_;
};
}
} // End of namespace samson.system

#endif  // ifndef _H_SAMSON_system_PROCESS_COMPONENT_MANAGER
