#include "samson_system/SpreadMapProcess.h" // Own interface

namespace samson {
namespace system {

const std::string SpreadMapProcess::kNullField = "null_field";

bool SpreadMapProcess::Update(Value *key, Value *state, Value **values, size_t num_values,
    samson::KVWriter* const writer) {

  if (key->CheckMapValue("app", name().c_str())) {

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
      if ((keys[i] == "app") || (include_field_ && (keys[i] == additional_field_))) {
        continue;
      }
      new_key.SetAsMap();
      new_key.AddValueToMap("app")->SetString(out_app_name_);
      new_key.AddValueToMap("concept")->SetString(keys[i]);

      Value *p_value = key->GetValueFromMap(keys[i].c_str());

      if (p_value->IsMap()) {
        LM_E(("Error, don't know how to distribute concept '%s' being a map", keys[i].c_str()));
        continue;
      } else if (p_value->IsVector()) {
        size_t value_vector_size = p_value->GetVectorSize();
        for (size_t j = 0; (j < value_vector_size); ++j) {
          new_value.SetAsMap();
          new_value.AddValueToMap("item")->SetString(p_value->GetValueFromVector(j)->GetString());
          if (include_field_) {
            new_value.AddValueToMap(additional_field_)->copyFrom(additional_field_value);
          }
          // In system.Value paradigm, input values are supposed to be 1.0, no fields information
          for (size_t k = 0; (k < num_values); ++k) {
            EmitFeedback(&new_key, &new_value, writer);
          }
        }
      } else {
        new_value.SetAsMap();
        new_value.AddValueToMap("item")->SetString(p_value->GetString());
        if (include_field_) {
          new_value.AddValueToMap(additional_field_)->copyFrom(additional_field_value);
        }
        // In system.Value paradigm, input values are supposed to be 1.0, no fields information
        for (size_t j = 0; (j < num_values); j++) {
          EmitFeedback(&new_key, &new_value, writer);
        }
      }
    }

    if (out_def_name() != SpreadMapProcess::kNullDest) {
      // Just reemit key-value pairs to the default output stream
      key->SetStringForMap("app", out_def_name().c_str());
      for (size_t j = 0; (j < num_values); j++) {
        EmitFeedback(key, values[j], writer);
      }
    }
    return true;
  } else {
    // Input key-value not processed because this is not process "app"
    return false;
  }
}
}
} // End of namespace
