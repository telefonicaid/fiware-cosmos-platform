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
 * FILE            HitCountProcess.cpp
 *
 * AUTHOR          Gregorio Escalada
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Definition of HitCountProcess class methods to keep
 *  an updated and time filtered count of hits,
 *  under the Process paradigm
 *
 */

#include "samson_system/HitCountProcess.h"   // Own interface

#include "au/log/LogMain.h"
#include "samson_system/Value.h"


namespace samson {
namespace system {
const std::string HitCountProcess::kNullDest("null");

void HitCountProcess::AddUpdateCountFunction(const std::string& name, double time_span, int n_top_items) {
  UpdateCountFunction update_count_function(name, time_span, n_top_items);

  counts_.push_back(update_count_function);
}

bool HitCountProcess::Update(Value *key, Value *state, Value **values, size_t num_values,
                             samson::KVWriter *const writer) {
  Value *p_hit;

  std::string input_item;
  std::string item;

  size_t newest_timestamp = 0;

  if (key->CheckMapValue(Value::kAppField.c_str(), name().c_str())) {
    if (key->GetStringFromMap(Value::kConceptField.c_str()) == NULL) {
      LOG_SE(("HitCountProcess. Error, no field 'concept' found in key:'%s'", key->str().c_str()));
      LM_E(("Error, no field 'concept' found in key:'%s'", key->str().c_str()));
      return false;
    }
    if (state->IsVoid()) {
      state->SetAsMap();
      state->SetDoubleForMap(Value::kGlobalCountField.c_str(), 0.0);
      Value *p_vector_profile = state->AddValueToMap(Value::kVectorProfileField);
      p_vector_profile->SetAsVector();
      for (size_t count_pos = 0; (count_pos < counts_.size()); count_pos++) {
        std::string hits_name = "prof_" + counts_[count_pos].name();
        Value *p_profile = p_vector_profile->AddValueToVector();
        p_profile->SetStringForMap(Value::kNameField.c_str(), hits_name.c_str());
        p_profile->SetDoubleForMap(Value::kUpdatedCountField.c_str(), 0.0);
        p_profile->SetDoubleForMap(Value::kTimestampField.c_str(), 0.0);
        p_profile->AddValueToMap(Value::kHitsField)->SetAsVector();
      }
    }

    for (size_t i = 0; (i < num_values); ++i) {
      if (values[i]->GetStringFromMap(Value::kItemField.c_str()) == NULL) {
        LOG_SW(("No item:'%s' in incoming value", Value::kItemField.c_str()));
        continue;
      }

      input_item = values[i]->GetStringFromMap(Value::kItemField.c_str());
      size_t timestamp = values[i]->GetDoubleFromMap(Value::kTimestampField.c_str());
      if (timestamp == 0) {
        LOG_SW(("Warning, for concept:'%s', item:'%s' timestamp==0 ", input_item.c_str(),
                key->GetStringFromMap(Value::kConceptField.c_str())));
      }
      if (timestamp > newest_timestamp) {
        newest_timestamp = timestamp;
      }
      // vector_profiles must be aligned with UpdateCount functions
      for (size_t count_pos = 0;
          (count_pos < state->GetValueFromMap(Value::kVectorProfileField.c_str())->GetVectorSize());
           ++count_pos)
      {
        if (state->GetValueFromMap(Value::kVectorProfileField.c_str()) == NULL) {
          LOG_SE(("HitCountProcess. Error, no 'vector_profile' field found in state"));
          LM_E(("Error, no 'vector_profile' field found in state"));
          return false;
        }
        Value *state_hits =
          state->GetValueFromMap(Value::kVectorProfileField.c_str())->GetValueFromVector(count_pos)->GetValueFromMap(
            Value::kHitsField.c_str());
        if (state_hits == NULL) {
          LOG_SE(("HitCountProcess. Error, no 'hits' field found in state"));
          return false;
        }

        Value *p_hit = NULL;
        bool found_hit = false;
        // When logs arrive in block from push, values are grouped,
        // so more efficient to look backwards.
        for (ssize_t j = state_hits->GetVectorSize() - 1; (j >= 0); --j) {
          p_hit = state_hits->GetValueFromVector(j);
          if (input_item == p_hit->GetStringFromMap(Value::kItemField.c_str())) {
            double old_count = p_hit->GetDoubleFromMap(Value::kCountField.c_str());
            size_t old_timestamp = p_hit->GetDoubleFromMap(Value::kTimestampField.c_str());
            double new_count = 1 + counts_[count_pos].UpdateCount(old_count, old_timestamp, timestamp);
            p_hit->SetDoubleForMap(Value::kTimestampField.c_str(), timestamp);
            p_hit->SetDoubleForMap(Value::kCountField.c_str(), new_count);
            found_hit = true;
            break;
          }
        }
        if (found_hit == false) {
          Value *new_hit = state_hits->AddValueToVector(state_hits->GetVectorSize());
          new_hit->SetAsMap();
          new_hit->AddValueToMap(Value::kItemField)->SetString(input_item);
          new_hit->AddValueToMap(Value::kTimestampField)->SetDouble(static_cast<double> (timestamp));
          new_hit->AddValueToMap(Value::kCountField)->SetDouble(1.0);
        }
      }
    }

    double new_global_count = state->GetDoubleFromMap(Value::kGlobalCountField.c_str());
    new_global_count += num_values;
    state->SetDoubleForMap(Value::kGlobalCountField.c_str(), new_global_count);

    // Last pass to update all counts to the latest timestamp
    // vector_profiles must be aligned with UpdateCount functions
    for (size_t count_pos = 0;
        (count_pos < state->GetValueFromMap(Value::kVectorProfileField.c_str())->GetVectorSize());
         ++count_pos)
    {
      Value *p_profile = state->GetValueFromMap(Value::kVectorProfileField.c_str())->GetValueFromVector(count_pos);
      double old_updated_count = p_profile->GetDoubleFromMap(Value::kUpdatedCountField.c_str());
      size_t old_time = p_profile->GetDoubleFromMap(Value::kTimestampField.c_str());
      double new_updated_count = num_values + counts_[count_pos].UpdateCount(old_updated_count, old_time,
                                                                             newest_timestamp);
      p_profile->SetDoubleForMap(Value::kUpdatedCountField.c_str(), new_updated_count);
      p_profile->SetDoubleForMap(Value::kTimestampField.c_str(), newest_timestamp);

      if ((newest_timestamp == 0) && (num_values > 0)) {
        LOG_SW(("Warning, for concept:'%s', newest_timestamp==0 with %lu values",
                key->GetStringFromMap(Value::kConceptField.c_str()), num_values));
      }
      Value *state_hits = p_profile->GetValueFromMap(Value::kHitsField.c_str());
      if (num_values > 0) {
        // We want only to update state count if we had had input values, and thus a newest_timestamp
        for (size_t j = 0; (j < state_hits->GetVectorSize()); ++j) {
          p_hit = state_hits->GetValueFromVector(j);
          double old_count = p_hit->GetDoubleFromMap(Value::kCountField.c_str());
          size_t old_timestamp = p_hit->GetDoubleFromMap(Value::kTimestampField.c_str());
          double new_count = counts_[count_pos].UpdateCount(old_count, old_timestamp, newest_timestamp);
          p_hit->SetDoubleForMap(Value::kTimestampField.c_str(), newest_timestamp);
          p_hit->SetDoubleForMap(Value::kCountField.c_str(), new_count);
        }
      }

      // Sort the vector on the Value::kCountField field of the hits.
      // Sort just the number of hits that will be transferred to the final state
      state_hits->PartialSortVectorOfMapsInDescendingOrder(Value::kCountField, counts_[count_pos].n_top_items());

      while (state_hits->GetVectorSize() > counts_[count_pos].n_top_items()) {
        state_hits->PopBackFromVector();
      }
    }

    Value new_key;
    new_key.AddValueToMap(Value::kAppField)->SetString(out_app_name().c_str());
    new_key.AddValueToMap(Value::kConceptField)->SetString(key->GetStringFromMap(Value::kConceptField.c_str()));

    EmitOutput(&new_key, state, writer);
    EmitState(key, state, writer);

    if (out_def_name() != HitCountProcess::kNullDest) {
      key->SetStringForMap(Value::kAppField.c_str(), out_def_name().c_str());
      for (size_t j = 0; (j < num_values); j++) {
        EmitFeedback(key, values[j], writer);
      }
    }
    return true;
  } else {
    return false;
  }
}
}
}   // End of namespace
