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
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_system_map2
#define _H_SAMSON_system_map2

#include <samson/module/samson.h>
#include <samson_system/Value.h>

namespace samson {
namespace system {

class map2 : public samson::Map {
    std::string command;

    samson::system::ValueContainer keyContainer;
    samson::system::ValueContainer valueContainer;

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

    void init(samson::KVWriter *writer) {
      // Setup the process chain...
      command = environment->get("command", "");
      au::ErrorManager error;
      filters_collection.AddFilters(command, writer, NULL, &error);

      // Error defined by user
      if (error.IsActivated())
        tracer->setUserError(error.GetMessage());
    }

    void run(samson::KVSetStruct* inputs, samson::KVWriter *writer) {
      for (size_t i = 0; i < inputs[0].num_kvs; i++) {
        keyContainer.value->parse(inputs[0].kvs[i]->key);
        valueContainer.value->parse(inputs[0].kvs[i]->value);

        // ---------------------------------------------------------
        // Run the filters
        // ---------------------------------------------------------

        KeyValue kv(keyContainer.value, valueContainer.value);
        filters_collection.run(kv);
      }
    }

    void finish(samson::KVWriter *writer) {
    }

};

} // end of namespace system
} // end of namespace samson

#endif
