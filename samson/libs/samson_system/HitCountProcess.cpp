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

#include "samson_system/Value.h"

namespace samson {
namespace system {

const std::string HitCountProcess::kNullDest("null");

void HitCountProcess::AddUpdateCountFunction(const std::string& name, double time_span, int n_top_items) {
  UpdateCountFunction update_count_function(name, time_span, n_top_items);
  counts_.push_back(update_count_function);
}

bool HitCountProcess::Update(Value *key, Value *state, Value **values, size_t num_values,
                             samson::KVWriter* const writer) {
  Value *p_hit;

  std::string input_item;
  std::string item;

  size_t newest_timestamp = 0;

  if (key->CheckMapValue("app", name().c_str())) {
    LM_M(("Detected app:'%s'", name().c_str()));
    if (key->GetStringFromMap("concept") == NULL) {
      LM_E(("Error, no field 'concept' found in key:'%s'", key->str().c_str()));
      return false;
    }
    if (state->IsVoid()) {
      LM_M(("Init state for key app:'%s', concept:'%s'", name().c_str(), key->GetStringFromMap("concept")));
      state->SetAsMap();
      state->SetDoubleForMap("global_count", 0.0);
      Value *p_vector_profile = state->AddValueToMap("vector_profile");
      p_vector_profile->SetAsVector();
      for (size_t count_pos = 0; (count_pos < counts_.size()); count_pos++) {
        std::string hits_name = "prof_" + counts_[count_pos].name();
        Value *p_profile = p_vector_profile->AddValueToVector();
        p_profile->SetStringForMap("name", hits_name.c_str());
        p_profile->SetDoubleForMap("updated_count", 0.0);
        p_profile->SetDoubleForMap("time", 0.0);
        p_profile->AddValueToMap("hits")->SetAsVector();
      }
    } else {
      // LM_M(("Existing state for key app:'%s', concept:'%s' (%lu global_count), with %lu items", name().c_str(), key->GetStringFromMap("concept"), state->GetDoubleFromMap("global_count"), state->GetValueFromMap("hits")->GetVectorSize()));
    }

    for (size_t i = 0; (i < num_values); ++i) {
      if (values[i]->GetStringFromMap("item") == NULL) {
        continue;
      }
      input_item = values[i]->GetStringFromMap("item");
      size_t timestamp = values[i]->GetDoubleFromMap("time");
      if (timestamp == 0) {
        LM_W(("Warning, for concept:'%s', item:'%s' timestamp==0 ", input_item.c_str(),
                key->GetStringFromMap("concept")));
      }
      if (timestamp > newest_timestamp) {
        newest_timestamp = timestamp;
      }
      // vector_profiles must be aligned with UpdateCount functions
      for (size_t count_pos = 0; (count_pos < state->GetValueFromMap("vector_profile")->GetVectorSize()); ++count_pos) {
        if (state->GetValueFromMap("vector_profile") == NULL) {
          LM_E(("Error, no 'vector_profile' field found in state"));
          return false;
        }
        Value *state_hits = state->GetValueFromMap("vector_profile")->GetValueFromVector(count_pos)->GetValueFromMap("hits");
        if (state_hits == NULL) {
          LM_E(("Error, no 'hits' field found in state"));
          return false;
        }

        Value *p_hit = NULL;
        bool found_hit = false;
        // When logs arrive in block from push, values are grouped,
        // so more efficient to look backwards.
        for (ssize_t j = state_hits->GetVectorSize() - 1; (j >= 0); --j) {
          p_hit = state_hits->GetValueFromVector(j);
          if (input_item == p_hit->GetStringFromMap("item")) {
            double old_count = p_hit->GetDoubleFromMap("count");
            size_t old_timestamp = p_hit->GetDoubleFromMap("time");
            double new_count = 1 + counts_[count_pos].UpdateCount(old_count, old_timestamp, timestamp);
            p_hit->SetDoubleForMap("time", timestamp);
            p_hit->SetDoubleForMap("count", new_count);
            // LM_M(("Particular update count for item:'%s'(%lu of %lu), old_count:%lf, old_time:%lu, new_count:%lf, new_time:%lu",  input_item.c_str(), j, state_hits->GetVectorSize(), old_count, old_timestamp, new_count, timestamp));

            found_hit = true;
            break;
          }
        }
        if (found_hit == false) {
          Value *new_hit = state_hits->AddValueToVector(state_hits->GetVectorSize());
          new_hit->SetAsMap();
          new_hit->AddValueToMap("item")->SetString(input_item);
          new_hit->AddValueToMap("time")->SetDouble(static_cast<double> (timestamp));
          new_hit->AddValueToMap("count")->SetDouble(1.0);
          // LM_M(("Added to state item:'%s'(count:%lf, time:%lf), now size:%lu", input_item.c_str(), new_hit->GetDoubleFromMap("count", 0.0), new_hit->GetDoubleFromMap("time", 0.0), state_hits->GetVectorSize()));
        }
      }
    }

    double new_global_count = state->GetDoubleFromMap("global_count");
    new_global_count += num_values;
    state->SetDoubleForMap("global_count", new_global_count);

    // Last pass to update all counts to the latest timestamp
    // vector_profiles must be aligned with UpdateCount functions
    for (size_t count_pos = 0; (count_pos < state->GetValueFromMap("vector_profile")->GetVectorSize()); ++count_pos) {
      Value *p_profile = state->GetValueFromMap("vector_profile")->GetValueFromVector(count_pos);
      double old_updated_count = p_profile->GetDoubleFromMap("updated_count");
      size_t old_time = p_profile->GetDoubleFromMap("time");
      double new_updated_count = num_values + counts_[count_pos].UpdateCount(old_updated_count, old_time, newest_timestamp);
      p_profile->SetDoubleForMap("updated_count", new_updated_count);
      p_profile->SetDoubleForMap("time", newest_timestamp);

      if (newest_timestamp == 0) {
        LM_W(("Warning, for concept:'%s', newest_timestamp==0 with %lu values",
                key->GetStringFromMap("concept"), num_values));
      }
      Value *state_hits = p_profile->GetValueFromMap("hits");
      if (num_values > 0) {
        // We want only to update state count if we had had input values, and thus a newest_timestamp
        for (size_t j = 0; (j < state_hits->GetVectorSize()); ++j) {
          p_hit = state_hits->GetValueFromVector(j);
          double old_count = p_hit->GetDoubleFromMap("count");
          size_t old_timestamp = p_hit->GetDoubleFromMap("time");
          double new_count = counts_[count_pos].UpdateCount(old_count, old_timestamp, newest_timestamp);
          p_hit->SetDoubleForMap("time", newest_timestamp);
          p_hit->SetDoubleForMap("count", new_count);
          // LM_M(("General update count for item:'%s'(%lu of %lu), old_count:%lf, old_time:%lu, new_count:%lf, new_time:%lu",  p_hit->GetStringFromMap("item"), j, state_hits->GetVectorSize(), old_count, old_timestamp, new_count, newest_timestamp));
        }
      }

      // Sort the vector on the "count" field of the hits.
      // Sort just the number of hits that will be transferred to the final state
      state_hits->PartialSortVectorOfMapsInDescendingOrder("count", counts_[count_pos].n_top_items());

      while (state_hits->GetVectorSize() > counts_[count_pos].n_top_items()) {
        // LM_M(("Pruning items from size:%lu to %lu", state_hits->GetVectorSize(), n_top_items_));
        state_hits->PopBackFromVector();
      }
      // LM_M(("End value sort and prune phase for %lu items", state_hits->GetVectorSize()));

      for (size_t i = 0; (i < state_hits->GetVectorSize()); ++i) {
        // LM_M(("Pruned state for concept:'%s,  prof:'%s', item i(%lu,'%s',%lf) of %lu", key->GetStringFromMap("concept"), p_profile->GetStringFromMap("name"), i, state_hits->GetValueFromVector(i)->GetStringFromMap("item"), state_hits->GetValueFromVector(i)->GetDoubleFromMap("count"), state_hits->GetVectorSize()));
      }
    }

    Value new_key;
    new_key.AddValueToMap("app")->SetString(out_app_name().c_str());
    new_key.AddValueToMap("concept")->SetString(key->GetStringFromMap("concept"));

    // LM_M(("Before emitting output  for concept:'%s' with %lu items", new_key_container.value->GetStringFromMap("concept"), state_hits->GetVectorSize()));

    EmitOutput(&new_key, state, writer);

    EmitState(key, state, writer);

    if (out_def_name() != HitCountProcess::kNullDest) {
      key->SetStringForMap("app", out_def_name().c_str());
      for (size_t j = 0; (j < num_values); j++) {
        EmitFeedback(key, values[j], writer);
      }
    }
    return true;
  } else {
    // LM_M(("key app:'%s' different from expected", key->GetStringFromMap("app"), name().c_str()));
    return false;
  }
}
}
}   // End of namespace
