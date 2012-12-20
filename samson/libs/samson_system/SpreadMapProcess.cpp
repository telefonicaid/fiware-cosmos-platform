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
 * FILE            SpreadMapProcess.cpp
 *
 * AUTHOR          Gregorio Escalada
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Definition of SpreadMapProcess class methods to emit
 *  all the fields from a system.Value key, under the Process paradigm.
 *  The inclusion of a reference field (Value::kTimestampField, "timestamp" usually) is optional
 *
 */

#include "samson_system/SpreadMapProcess.h"   // Own interface

#include "au/log/LogMain.h"

namespace samson {
namespace system {

const std::string SpreadMapProcess::kNullField("null_field");
const std::string SpreadMapProcess::kNullDest("null");

bool SpreadMapProcess::Update(Value *key, Value *state, Value **values, size_t num_values,
                              samson::KVWriter* const writer) {
  if (key->CheckMapValue(Value::kAppField.c_str(), name().c_str())) {
    //LOG_SM(("SpreadMapProcess. Detected app:'%s' with %lu values in key:'%s'", name().c_str(), num_values,
    //        key->str().c_str()));
    Value *additional_field_value = NULL;
    if (include_field_) {
      // If additional field, we must check it is present among keys
      additional_field_value = key->GetValueFromMap(additional_field_.c_str());
      if (additional_field_value == NULL) {
        LM_E(("Error, no key additional field:'%s'", additional_field_.c_str()));
        EmitLog("Error", au::str("Error, no key additional field:'%s'", additional_field_.c_str()).c_str(), writer);
        return false;
      }
    }

    Value new_key;
    Value new_value;

    std::vector<std::string> keys = key->GetKeysFromMap();
    size_t keys_size = keys.size();

    for (size_t i = 0; (i < keys_size); ++i) {
      if ((keys[i] == Value::kAppField) || (include_field_ && (keys[i] == additional_field_))) {
        continue;
      }
      new_key.SetAsMap();
      new_key.AddValueToMap(Value::kAppField)->SetString(out_app_name_);
      new_key.AddValueToMap(Value::kConceptField)->SetString(keys[i]);
      //LOG_SM(("SpreadMapProcess. Working with concept:'%s'", keys[i].c_str()));

      // As we are running through all the fileds in the key, we are sure p_value exists
      Value *p_value = key->GetValueFromMap(keys[i].c_str());

      if (p_value->IsMap()) {
        LOG_SE(("SpreadMapProcess. Error, don't know how to distribute concept '%s' being a map", keys[i].c_str()));
        LM_E(("Error, don't know how to distribute concept '%s' being a map", keys[i].c_str()));
        continue;
      } else if (p_value->IsVector()) {
        size_t value_vector_size = p_value->GetVectorSize();
        for (size_t j = 0; (j < value_vector_size); ++j) {
          new_value.SetAsMap();
          new_value.AddValueToMap(Value::kItemField)->SetString(p_value->GetValueFromVector(j)->GetString());
          if (include_field_) {
            new_value.AddValueToMap(additional_field_)->copyFrom(additional_field_value);
          }
          // In system.Value paradigm, input values are supposed to be 1.0, no fields information
          for (size_t k = 0; (k < num_values); ++k) {
            //LOG_SM(("SpreadMapProcess: Emit feedback, key:'%s', vector value:'%s'", new_key.str().c_str(),
            //        new_value.str().c_str()));
            EmitFeedback(&new_key, &new_value, writer);
          }
        }
      } else {
        new_value.SetAsMap();
        new_value.AddValueToMap(Value::kItemField)->SetString(p_value->GetString());
        if (include_field_) {
          new_value.AddValueToMap(additional_field_)->copyFrom(additional_field_value);
        }
        // In system.Value paradigm, input values are supposed to be 1.0, no fields information
        for (size_t j = 0; (j < num_values); j++) {
          //LOG_SM(("SpreadMapProcess: Emit feedback, key:'%s', value:'%s'", new_key.str().c_str(),
          //        new_value.str().c_str()));
          EmitFeedback(&new_key, &new_value, writer);
        }
      }
    }

    if (out_def_name() != SpreadMapProcess::kNullDest) {
      // Just reemit key-value pairs to the default output stream
      key->SetStringForMap(Value::kAppField.c_str(), out_def_name().c_str());
      for (size_t j = 0; (j < num_values); j++) {
        //LOG_SM(("SpreadMapProcess: Emit next flow feedback, key:'%s', value:'%s'", key->str().c_str(),
        //        values[j]->str().c_str()));
        EmitFeedback(key, values[j], writer);
      }
    }
    return true;
  } else {
    // Input key-value not processed because this is not process Value::kAppField
    return false;
  }
}
}
}   // End of namespace
