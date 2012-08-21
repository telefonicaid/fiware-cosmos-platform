

#ifndef _H_SAMSON_system_TOP_PROCESS_COMPONENT
#define _H_SAMSON_system_TOP_PROCESS_COMPONENT

#include "samson_system/Value.h"

namespace samson { namespace system {
                   class TopProcessComponent : public ProcessComponent {
                     int time_span;
                     int top_list_max_size;
                     double forgetting_factor;

public:

                     TopProcessComponent() : ProcessComponent("top") {
                       // time_span = environment->get( "time_span" ,  300 ); // By default 5 minuts average
                       time_span = 300;
                       top_list_max_size = 100;

                       // Compute forgetting factor based on time-span
                       forgetting_factor = ((double)(time_span - 1)) / ((double)time_span);
                     }

                     static void emit_top_element(const char *concept, const char *category, double counter, samson::KVWriter *writer) {
                       ValueContainer keyContainer;
                       ValueContainer valueContainer;

                       keyContainer.value->set_string_for_map("app", "top");
                       keyContainer.value->set_string_for_map("concept", concept);
                       keyContainer.value->set_string_for_map("category", category);
                       valueContainer.value->set_double(counter);

                       emit_feedback(keyContainer.value, valueContainer.value, writer);
                     }

                     // Update this state based on input values
                     bool update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter *writer) {
                       if (key->checkMapValue("app", "top")) {
                         update_accumulator(key, state, values, num_values, writer);
                         return true;
                       }

                       if (key->checkMapValue("app", "top.category")) {
                         update_category(key, state, values, num_values, writer);
                         return true;
                       }
                       return false;
                     }

                     void update_accumulator(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter *writer) {
                       emit_log("debug",
                                au::str("Processing top.accumulateion state %s - %s with %lu values", key->str().c_str(),
                                        state->str().c_str(), num_values), writer);

                       if (num_values == 0) {
                         // Emit state as it is
                         emit_state(key, state, writer);
                         return;
                       }

                       const char *category = key->get_string_from_map("category");
                       const char *concept  = key->get_string_from_map("concept");

                       if (!category || !concept) {
                         return; // Incorrect key for this process component
                       }
                       double state_total = state->get_double_from_map("total", 0);
                       size_t state_time = state->get_uint64_from_map("time");

                       // Update time and total based on the new time stamp
                       size_t current_time = ::time(NULL);
                       if (current_time > state_time) {
                         ::time_t diff =  current_time - state_time;
                         state_total = (((double)state_total ) * pow(forgetting_factor, (double)diff));
                         state_time  = current_time;
                       }

                       // Add new samples
                       for (size_t i = 0; i < num_values; i++) {
                         double tmp_value = values[i]->get_double(0); // Get double value ( 0 as default if this is not a number )
                         state_total += tmp_value;
                       }

                       // Reconstruct state
                       state->set_double_for_map("total", state_total);
                       state->set_uint64_for_map("time", state_time);
                       emit_state(key, state, writer);


                       // Emit to be considered in the top list of categories
                       ValueContainer key_emit_container;
                       ValueContainer value_emit_container;
                       key_emit_container.value->set_string_for_map("app", "top.category");
                       key_emit_container.value->set_string_for_map("category", key->get_string_from_map("category"));
                       value_emit_container.value->copyFrom(state);
                       value_emit_container.value->set_string_for_map("concept", key->get_string_from_map("concept"));
                       emit_feedback(key_emit_container.value, value_emit_container.value, writer);
                     }

                     void update_category_with_value(Value *state, Value *new_value) {
                       double new_value_total = new_value->get_double_from_map("total", 0);
                       const char *new_value_concept = new_value->get_string_from_map("concept");

                       if (!new_value_concept) {
                         return; // Skip incorrect value
                       }
                       // Search for the same concept in the top list....

                       for (size_t p = 0; p < state->get_vector_size(); p++) {
                         Value *vector_value = state->get_value_from_vector(p);
                         const char *vector_value_concept = vector_value->get_string_from_map("concept");

                         if (strcmp(vector_value_concept, new_value_concept) == 0) {
                           // Same concept, update element
                           vector_value->copyFrom(new_value);

                           // Recorder this element in the vector
                           while ((p > 0) &&
                                  ( state->get_value_from_vector(p)->get_double_from_map("total",
                                                                                         0) >
                                    state->get_value_from_vector(p - 1)->get_double_from_map("total", 0))) {
                             // Swap p and p-1 elements
                             state->swap_vector_components(p, p - 1);
                             p--;
                           }

                           while ((p < (state->get_vector_size() - 1)) &&
                                  ( state->get_value_from_vector(p)->get_double_from_map("total",
                                                                                         0) <
                                    state->get_value_from_vector(p + 1)->get_double_from_map("total", 0))) {
                             // Swap p and p-1 elements
                             state->swap_vector_components(p, p + 1);
                             p++;
                           }

                           return;
                         }
                       }

                       // Search if I am a better result
                       for (size_t p = 0; p < state->get_vector_size(); p++) {
                         Value *vector_value = state->get_value_from_vector(p);

                         // const char* vector_value_concept = vector_value->get_string_from_map("concept");
                         double vector_value_total = vector_value->get_double_from_map("total");

                         if (new_value_total > vector_value_total) {
                           // Insert here
                           state->add_value_to_vector(p)->copyFrom(new_value); // Insert this value at this position

                           // Check length of the vector
                           while (state->get_vector_size() > 100) {
                             state->pop_back_from_vector();
                           }

                           return;
                         }
                       }

                       // Push at the end of the vector if possible
                       if (state->get_vector_size() < 100) {
                         state->add_value_to_vector()->copyFrom(new_value);
                       }
                     }

                     void update_category(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter *writer) {
                       emit_log("debug",
                                au::str("Processing top.category state %s - %s with %lu values", key->str().c_str(), state->str().c_str(),
                                        num_values), writer);

                       if (num_values == 0) {
                         // Emit state as it is
                         emit_state(key, state, writer);
                         return;
                       }

                       const char *category = key->get_string_from_map("category");

                       if (!category) {
                         return; // Incorrect key for this process component
                       }
                       // Process new elements
                       for (size_t i = 0; i < num_values; i++) {
                         emit_log("debug", au::str("Updating state %s with new value %s", state->str().c_str(),
                                                   values[i]->str().c_str()), writer);
                         update_category_with_value(state, values[i]);
                         emit_log("debug", au::str("         state %s", state->str().c_str()), writer);
                       }

                       // Emit new state
                       emit_state(key, state, writer);
                     }
                   };
                   } } // End of namespace

#endif // ifndef _H_SAMSON_system_TOP_PROCESS_COMPONENT