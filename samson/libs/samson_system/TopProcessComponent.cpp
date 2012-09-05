#include "samson_system/TopProcessComponent.h"

#include <math.h>

#include "au/string.h"
#include "samson_system/ValueContainer.h"

namespace samson {
namespace system {

void TopProcessComponent::EmitTopElement(const char *concept, const char *category, double counter,
    samson::KVWriter* const writer) {
  ValueContainer keyContainer;
  ValueContainer valueContainer;

  keyContainer.value->SetStringForMap("app", "top");
  keyContainer.value->SetStringForMap("concept", concept);
  keyContainer.value->SetStringForMap("category", category);
  valueContainer.value->SetDouble(counter);

  EmitFeedback(keyContainer.value, valueContainer.value, writer);
}

// Update this state based on input values
bool TopProcessComponent::Update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer) {
  if (key->CheckMapValue("app", "top")) {
    UpdateAccumulator(key, state, values, num_values, writer);
    return true;
  }

  if (key->CheckMapValue("app", "top.category")) {
    UpdateCategory(key, state, values, num_values, writer);
    return true;
  }
  return false;
}

void TopProcessComponent::UpdateAccumulator(Value *key, Value *state, Value **values, size_t num_values,
    samson::KVWriter* const writer) {
  EmitLog("debug",
          au::str("Processing top.accumulateion state %s - %s with %lu values", key->str().c_str(),
                  state->str().c_str(), num_values), writer);

  if (num_values == 0) {
    // Emit state as it is
    EmitState(key, state, writer);
    return;
  }

  const char *category = key->GetStringFromMap("category");
  const char *concept = key->GetStringFromMap("concept");

  if (!category || !concept) {
    return; // Incorrect key for this process component
  }
  double state_total = state->GetDoubleFromMap("total", 0);
  size_t state_time = state->GetUint64FromMap("time");

  // Update time and total based on the new time stamp
  size_t current_time = ::time(NULL);
  if (current_time > state_time) {
    int diff = static_cast<int>(current_time - state_time);
    // In C++, pow(double, int) is faster than pow(double, double)
    state_total *= pow(forgetting_factor_, diff);
    state_time = current_time;
  }

  // Add new samples
  for (size_t i = 0; i < num_values; ++i) {
    double tmp_value = values[i]->GetDouble(0); // Get double value ( 0 as default if this is not a number )
    state_total += tmp_value;
  }

  // Reconstruct state
  state->SetDoubleForMap("total", state_total);
  state->SetUint64ForMap("time", state_time);
  EmitState(key, state, writer);

  // Emit to be considered in the top list of categories
  ValueContainer key_emit_container;
  ValueContainer value_emit_container;
  key_emit_container.value->SetStringForMap("app", "top.category");
  key_emit_container.value->SetStringForMap("category", key->GetStringFromMap("category"));
  value_emit_container.value->copyFrom(state);
  value_emit_container.value->SetStringForMap("concept", key->GetStringFromMap("concept"));
  EmitFeedback(key_emit_container.value, value_emit_container.value, writer);
}

void TopProcessComponent::UpdateCategoryWithValue(Value *state, Value *new_value) {
  double new_value_total = new_value->GetDoubleFromMap("total", 0);
  const char *new_value_concept = new_value->GetStringFromMap("concept");

  if (!new_value_concept) {
    return; // Skip incorrect value
  }
  // Search for the same concept in the top list....

  for (size_t p = 0; p < state->GetVectorSize(); ++p) {
    Value *vector_value = state->GetValueFromVector(p);
    const char *vector_value_concept = vector_value->GetStringFromMap("concept");

    if (strcmp(vector_value_concept, new_value_concept) == 0) {
      // Same concept, update element
      vector_value->copyFrom(new_value);

      // Recorder this element in the vector
      while ((p > 0) && (state->GetValueFromVector(p)->GetDoubleFromMap("total", 0)
          > state->GetValueFromVector(p - 1)->GetDoubleFromMap("total", 0))) {
        // Swap p and p-1 elements
        state->SwapVectorComponents(p, p - 1);
        --p;
      }

      while ((p < (state->GetVectorSize() - 1)) && (state->GetValueFromVector(p)->GetDoubleFromMap("total", 0)
          < state->GetValueFromVector(p + 1)->GetDoubleFromMap("total", 0))) {
        // Swap p and p-1 elements
        state->SwapVectorComponents(p, p + 1);
        ++p;
      }

      return;
    }
  }

  // Search if I am a better result
  for (size_t p = 0; p < state->GetVectorSize(); ++p) {
    Value *vector_value = state->GetValueFromVector(p);

    // const char* vector_value_concept = vector_value->GetStringFromMap("concept");
    double vector_value_total = vector_value->GetDoubleFromMap("total");

    if (new_value_total > vector_value_total) {
      // Insert here
      state->AddValueToVector(p)->copyFrom(new_value); // Insert this value at this position

      // Check length of the vector
      while (state->GetVectorSize() > top_list_max_size_) {
        state->PopBackFromVector();
      }

      return;
    }
  }

  // Push at the end of the vector if possible
  if (state->GetVectorSize() < top_list_max_size_) {
    state->AddValueToVector()->copyFrom(new_value);
  }
}

void TopProcessComponent::UpdateCategory(Value *key, Value *state, Value **values, size_t num_values,
    samson::KVWriter* const writer) {
  EmitLog("debug",
          au::str("Processing top.category state %s - %s with %lu values", key->str().c_str(), state->str().c_str(),
                  num_values), writer);

  if (num_values == 0) {
    // Emit state as it is
    EmitState(key, state, writer);
    return;
  }

  const char *category = key->GetStringFromMap("category");

  if (!category) {
    return; // Incorrect key for this process component
  }
  // Process new elements
  for (size_t i = 0; i < num_values; i++) {
    EmitLog("debug", au::str("Updating state %s with new value %s", state->str().c_str(), values[i]->str().c_str()),
            writer);
    UpdateCategoryWithValue(state, values[i]);
    EmitLog("debug", au::str("         state %s", state->str().c_str()), writer);
  }

  // Emit new state
  EmitState(key, state, writer);
}

}
} // End of namespace

