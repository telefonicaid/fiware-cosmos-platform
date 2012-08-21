

#include "samson_system/ValueContainer.h"

#include "ProcessComponent.h"  // Own interface

namespace samson { namespace system {
                   void emit_output(Value *key, Value *state, samson::KVWriter *writer) {
                     // Debug
                     emit_log("debug", au::str("Emit output %s - %s", key->str().c_str(), state->str().c_str()), writer);

                     // Emit using 0- channel
                     writer->emit(0, key, state);
                   }

                   void emit_feedback(Value *key, Value *state, samson::KVWriter *writer) {
                     // Debug
                     emit_log("debug", au::str("Emit feedback %s - %s", key->str().c_str(), state->str().c_str()), writer);

                     // Emit using 1- channel
                     writer->emit(1, key, state);
                   }

                   void emit_state(Value *key, Value *state, samson::KVWriter *writer) {
                     // Debug
                     emit_log("debug", au::str("Emit state %s - %s", key->str().c_str(), state->str().c_str()), writer);

                     // Emit using 2- channel
                     writer->emit(2, key, state);
                   }

                   void emit_log(const std::string& key, const std::string& message, samson::KVWriter *writer) {
                     samson::system::ValueContainer key_message;
                     samson::system::ValueContainer value_message;

                     key_message.value->set_string(key);
                     value_message.value->set_string(message);

                     writer->emit(-1, key_message.value, value_message.value);
                   }
                   } }  // End of namespace