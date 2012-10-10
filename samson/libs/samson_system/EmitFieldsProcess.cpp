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
 * FILE            EmitFieldsProcess.cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Definition of EmitFieldsProcess class methods to select and emit
 *  fields from a system.Value key, under the Process paradigm
 *
 */

#include "samson_system/EmitFieldsProcess.h"   // Own interface

namespace samson {
namespace system {

const std::string EmitFieldsProcess::kNullField("null_field");
const std::string EmitFieldsProcess::kNullDest("null");

bool EmitFieldsProcess::Update(Value *key, Value *state, Value **values, size_t num_values,
                               samson::KVWriter* const writer) {
  if (key->CheckMapValue(Value::kAppField.c_str(), name().c_str())) {
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

    std::vector<Value *> independent_values;
    if (include_independent_) {
      // If independent field, we must check it is present among keys
      Value *p_field = key->GetValueFromMap(independent_concept_.c_str());
      if (p_field == NULL) {
        LM_E(("Error, no key independent field:'%s'", independent_concept_.c_str()));
        EmitLog("Error", au::str("Error, no key independent field:'%s'", independent_concept_.c_str()).c_str(), writer);
        return false;
      }

      if (p_field->IsVector()) {
        size_t vector_size = p_field->GetVectorSize();
        for (size_t position = 0; (position < vector_size); ++position) {
          independent_values.push_back(p_field->GetValueFromVector(position));
        }
      } else {
        independent_values.push_back(p_field);
      }
    }

    Value new_key;
    Value new_value;

    for (size_t i = 0; (i < concepts_.size()); ++i) {
      Value *p_value = key->GetValueFromMap(concepts_[i].c_str());
      if (p_value == NULL) {
        LM_W(("Error, no key concept field:'%s'", concepts_[i].c_str()));
        EmitLog("Error", au::str("Error, no key concept field:'%s'", concepts_[i].c_str()).c_str(), writer);
        continue;
      }

      new_key.SetAsMap();
      new_key.AddValueToMap(Value::kAppField)->SetString(out_app_name_);
      new_key.AddValueToMap(Value::kConceptField)->SetString(concepts_[i]);

      if (p_value->IsMap()) {
        LM_E(("Error, don't know how to distribute concept '%s' being a map", concepts_[i].c_str()));
        continue;
      } else if (p_value->IsVector()) {
        size_t value_vector_size = p_value->GetVectorSize();
        for (size_t j = 0; (j < value_vector_size); ++j) {
          new_value.SetAsMap();
          new_value.AddValueToMap(Value::kConceptField)->SetString(concepts_[i]);
          new_value.AddValueToMap(Value::kItemField)->SetString(p_value->GetValueFromVector(j)->GetString());
          if (include_field_) {
            new_value.AddValueToMap(additional_field_)->copyFrom(additional_field_value);
          }
          if (include_independent_) {
            size_t independent_size = independent_values.size();
            for (size_t k = 0; (k < independent_size); ++k) {
              new_key.AddValueToMap("member")->copyFrom(independent_values[k]);
              for (size_t j = 0; (j < num_values); j++) {
                // LM_M(("Emit output, key:'%s', value:'%s'", new_key.str().c_str(), new_value.str().c_str()));
                EmitFeedback(&new_key, &new_value, writer);
              }
            }
          } else {
            for (size_t j = 0; (j < num_values); j++) {
              // LM_M(("Emit output, key:'%s', value:'%s'", new_key.str().c_str(), new_value.str().c_str()));
              EmitFeedback(&new_key, &new_value, writer);
            }
          }
        }
      } else {
        // new_value.SetAsVoid();
        new_value.SetAsMap();
        new_value.AddValueToMap(Value::kConceptField)->SetString(concepts_[i]);
        new_value.AddValueToMap(Value::kItemField)->SetString(p_value->GetString());
        if (include_field_) {
          new_value.AddValueToMap(additional_field_)->copyFrom(additional_field_value);
        }
        if (include_independent_) {
          size_t independent_size = independent_values.size();
          for (size_t k = 0; (k < independent_size); ++k) {
            // We add the values to the key to have a state profile per user, or category...
            new_key.AddValueToMap("member")->copyFrom(independent_values[k]);
            for (size_t j = 0; (j < num_values); j++) {
              // LM_M(("Emit output, key:'%s', value:'%s'", new_key.str().c_str(), new_value.str().c_str()));
              EmitFeedback(&new_key, &new_value, writer);
            }
          }
        } else {
          for (size_t j = 0; (j < num_values); j++) {
            // LM_M(("Emit output, key:'%s', value:'%s'", new_key.str().c_str(), new_value.str().c_str()));
            EmitFeedback(&new_key, &new_value, writer);
          }
        }
      }
    }

    if (out_def_name() != EmitFieldsProcess::kNullDest) {
      // Just reemit key-value pairs to the default output stream
      key->SetStringForMap(Value::kAppField.c_str(), out_def_name().c_str());
      for (size_t j = 0; (j < num_values); j++) {
        // LM_M(("Emit feedback, key:'%s', value:'%s'", key->str().c_str(), values[j]->str().c_str()));
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
