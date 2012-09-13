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
 * FILE            ProcessComponent.cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Definition of ProcessComponent class methods to emit new system.Value key-value pairs
 *  to the three output queues (output, feedback and state), under the Process paradigm
 *
 */

#include "samson_system/ProcessComponent.h"  // Own interface

#include "samson_system/ValueContainer.h"

namespace samson {
namespace system {
void EmitOutput(Value *key, Value *state, samson::KVWriter* const writer) {
  // Debug
  EmitLog("debug", au::str("Emit output %s - %s", key->str().c_str(), state->str().c_str()), writer);

  // Emit using 0- channel
  writer->emit(0, key, state);
}

void EmitFeedback(Value *key, Value *state, samson::KVWriter* const writer) {
  // Debug
  EmitLog("debug", au::str("Emit feedback %s - %s", key->str().c_str(), state->str().c_str()), writer);

  // Emit using 1- channel
  writer->emit(1, key, state);
}

void EmitState(Value *key, Value *state, samson::KVWriter* const writer) {
  // Debug
  EmitLog("debug", au::str("Emit state %s - %s", key->str().c_str(), state->str().c_str()), writer);

  // Emit using 2- channel
  writer->emit(2, key, state);
}

void EmitLog(const std::string& key, const std::string& message, samson::KVWriter* const writer) {
  samson::system::ValueContainer key_message;
  samson::system::ValueContainer value_message;

  key_message.value->SetString(key);
  value_message.value->SetString(message);

  writer->emit(-1, key_message.value, value_message.value);
}
}
}   // End of namespace
