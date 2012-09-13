/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) 2012 Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/*
 * FILE            TopProcessComponent.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Declaration of TopProcessComponent class to update the count and emit the top
 *  hits under the Process paradigm. It operates on fixed fields in the input queue
 *  named "category", "concept", "total"
 *
 */
#ifndef _H_SAMSON_system_TOP_PROCESS_COMPONENT
#define _H_SAMSON_system_TOP_PROCESS_COMPONENT

#include "samson/module/KVWriter.h"
#include "samson_system/ProcessComponent.h"
#include "samson_system/Value.h"

namespace samson {
namespace system {

class TopProcessComponent : public ProcessComponent {
  public:
    TopProcessComponent() :
      ProcessComponent("top")
      , time_span_(300)
      , top_list_max_size_(100)
      , forgetting_factor_((static_cast<double> (time_span_ - 1)) / (static_cast<double> (time_span_))) {}

    static void EmitTopElement(const char *concept, const char *category, double counter, samson::KVWriter* const writer);

    // Update this state based on input values
    bool Update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);
    void UpdateAccumulator(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);
    void UpdateCategoryWithValue(Value *state, Value *new_value);
    void UpdateCategory(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);

  private:
    int time_span_;
    unsigned int top_list_max_size_;
    double forgetting_factor_;
};
}
}   // End of namespace

#endif  // ifndef _H_SAMSON_system_TOP_PROCESS_COMPONENT
