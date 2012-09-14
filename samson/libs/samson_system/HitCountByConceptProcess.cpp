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
 * FILE            HitCountByConceptProcess.cpp
 *
 * AUTHOR          Gregorio Escalada
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Definition of HitCountByConceptProcess class methods to keep
 *  an updated and time filtered count of hits related by a concept,
 *  under the Process paradigm
 *
 */

#include "samson_system/HitCountByConceptProcess.h"   // Own interface

#include "samson_system/Value.h"
#include "samson_system/ValueContainer.h"

namespace samson {
namespace system {

const std::string HitCountByConceptProcess::kNullDest("null");

void HitCountByConceptProcess::AddUpdateCountFunction(const std::string& name, double time_span, int n_top_items) {
  UpdateCountFunction update_count_function(name, time_span, n_top_items);
  counts_.push_back(update_count_function);
}

Value *HitCountByConceptProcess::GetConceptInInstantProfile(Value *p_instant_profile,
                                                            const std::string& concept) const {
  Value *p_concept_profile = NULL;

  if ((p_concept_profile = FindConceptInInstantProfile(p_instant_profile, concept)) != NULL) {
    return p_concept_profile;
  }
  p_instant_profile->SetAsVector();
  p_concept_profile = p_instant_profile->AddValueToVector();
  p_concept_profile->SetStringForMap("name", concept.c_str());
  p_concept_profile->SetDoubleForMap("global_count", 0.0);
  Value *p_vector_profile = p_concept_profile->AddValueToMap("vector_profile");
  p_vector_profile->SetAsVector();
  for (size_t count_pos = 0; (count_pos < counts_.size()); ++count_pos) {
    std::string hits_name = "prof_" + counts_[count_pos].name();
    Value *p_profile = p_vector_profile->AddValueToVector();
    p_profile->SetStringForMap("name", hits_name.c_str());
    p_profile->SetDoubleForMap("updated_count", 0.0);
    p_profile->SetDoubleForMap("time", 0.0);
    p_profile->AddValueToMap("hits")->SetAsVector();
  }
  return p_concept_profile;
}

Value *HitCountByConceptProcess::FindConceptInInstantProfile(Value *p_instant_profile, const std::string& concept) {
  if (p_instant_profile->IsVector() == false) {
    LM_E(("Error, Value *p_instant_profile is not a vector"));
    return NULL;
  }
  for (size_t i = 0; (i < p_instant_profile->GetVectorSize()); ++i) {
    Value *p_concept_profile = p_instant_profile->GetValueFromVector(i);
    if (p_concept_profile->GetStringFromMap("name") == concept) {
      return p_concept_profile;
    }
  }
  return NULL;
}

bool HitCountByConceptProcess::Update(Value *key, Value *state, Value **values, size_t num_values,
                                      samson::KVWriter* const writer) {
  Value *p_instant_profile;
  Value *p_hit;

  std::string val_item;
  std::string val_concept;
  std::string item;

  size_t newest_timestamp = 0;

  if (key->CheckMapValue("app", name().c_str())) {
    LM_M(("Detected app:'%s'", name().c_str()));
    if (key->GetStringFromMap("concept") == NULL) {
      LM_E(("Error, no field 'concept' found in key:'%s'", key->str().c_str()));
      return false;
    }

    if (state->IsVoid()) {
      LM_M(("Init state for key app:'%s', concept:'%s'", name().c_str(),
              key->GetStringFromMap("concept")));
      state->SetAsMap();
      p_instant_profile = state->AddValueToMap("instant_profile");
      p_instant_profile->SetAsVector();
    } else {
      p_instant_profile = state->GetValueFromMap("instant_profile");
      if (p_instant_profile == NULL) {
        LM_E(("Error, no 'instant_profile' field in state"));
      }
      return false;
      // LM_M(("Existing state for key app:'%s', concept:'%s' (%lu global_count), with %lu items", name().c_str(), key->GetStringFromMap("concept"), state->GetDoubleFromMap("global_count"), state->GetValueFromMap("hits")->GetVectorSize()));
    }

    for (size_t i = 0; (i < num_values); ++i) {
      if ((values[i]->GetStringFromMap("item") == NULL) || (values[i]->GetStringFromMap("concept") == NULL)) {
        continue;
      }
      val_concept = values[i]->GetStringFromMap("concept");

      Value *p_concept_profile = GetConceptInInstantProfile(p_instant_profile, val_concept);

      std::string input_item = values[i]->GetStringFromMap("item");
      size_t timestamp = values[i]->GetDoubleFromMap("time");
      if (timestamp == 0) {
        LM_W(("Warning, for concept:'%s', item:'%s' timestamp==0 ", val_concept.c_str(),
                input_item.c_str()));
      }
      if (timestamp > newest_timestamp) {
        newest_timestamp = timestamp;
      }
      for (size_t count_pos = 0; (count_pos < p_concept_profile->GetValueFromMap("vector_profile")->GetVectorSize()); ++count_pos) {
        Value *state_hits =
            p_concept_profile->GetValueFromMap("vector_profile")->GetValueFromVector(count_pos)-> GetValueFromMap("hits");
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

    for (size_t profile_pos = 0; (profile_pos < p_instant_profile->GetVectorSize()); ++profile_pos) {
      Value *p_concept_profile = p_instant_profile->GetValueFromVector(profile_pos);

      double new_global_count = p_concept_profile->GetDoubleFromMap("global_count");
      new_global_count += num_values;
      p_concept_profile->SetDoubleForMap("global_count", new_global_count);

      for (size_t count_pos = 0; (count_pos < p_concept_profile->GetValueFromMap("vector_profile")->GetVectorSize()); ++count_pos) {
        Value *p_profile = p_concept_profile->GetValueFromMap("vector_profile")->GetValueFromVector(count_pos);
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
    }

    ValueContainer new_key_container;
    new_key_container.value->AddValueToMap("app")->SetString(out_app_name().c_str());
    new_key_container.value->AddValueToMap("concept")->SetString(key->GetStringFromMap("concept"));

    // LM_M(("Before emiting output  for concept:'%s' with %lu items", new_key_container.value->GetStringFromMap("concept"), state_hits->GetVectorSize()));

    EmitOutput(new_key_container.value, state, writer);

    EmitState(key, state, writer);

    if (out_def_name() != HitCountByConceptProcess::kNullDest) {
      key->SetStringForMap("app", out_def_name().c_str());
      for (size_t j = 0; (j < num_values); ++j) {
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
