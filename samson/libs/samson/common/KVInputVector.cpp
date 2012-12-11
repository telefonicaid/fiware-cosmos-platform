/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "samson/common/KVInputVector.h"      // Own interface

#include <algorithm>            // std::sort
#include <string>
#include <vector>

#include "samson/module/ModulesManager.h"   // samson::ModulesManager
namespace samson {
#pragma mark KVInputVectorBase

void KVInputVectorBase::prepareInput(size_t _max_num_kvs) {
  if (_max_num_kvs > max_num_kvs) {
    if (_kv) {
      free(_kv);
    }
    if (kv) {
      free(kv);   // Set a new maximum number of kvs
    }
    max_num_kvs = _max_num_kvs;

    _kv = reinterpret_cast<KV **>(malloc(sizeof(*_kv) * _max_num_kvs));
    kv = reinterpret_cast<KV *>(malloc(sizeof(*kv) * max_num_kvs));
  }

  // Set the pointers to internal structure ( to sort again )
  for (size_t i = 0; i < _max_num_kvs; i++) {
    _kv[i] = &kv[i];
  }

  num_kvs = 0;
}

#pragma mark KVInputVector

KVInputVector::KVInputVector(Operation *operation) {
  // Take the number of inputs
  num_inputs_ = operation->getNumInputs();

  _kv = NULL;
  kv = NULL;

  max_num_kvs = 0;   // Maximum number of kvs to be processes
  num_kvs = 0;   // Current number of key-values pairs

  std::vector<KVFormat> inputFormats = operation->getInputFormats();

  if (inputFormats.size() == 0) {
    LOG_SW(("Operation %s has no inputs", operation->_name.c_str()));
    return;
  }

  Data *keyData = au::Singleton<ModulesManager>::shared()->GetData(inputFormats[0].keyFormat);
  if (!keyData) {
    LM_X(1, ("Internal error:"));
  }
  keyDataInstance_ = reinterpret_cast<DataInstance *>(keyData->getInstance());
  if (!keyDataInstance_) {
    LM_X(1, ("Internal error:"));   // Get the right functions to process input key-values
  }
  for (int i = 0; i < static_cast<int>(inputFormats.size()); i++) {
    Data *valueData = au::Singleton<ModulesManager>::shared()->GetData(inputFormats[i].valueFormat);
    if (!valueData) {
      LM_X(1, ("Internal error:"));
    }
    DataInstance *valueDataInstance = reinterpret_cast<DataInstance *>(valueData->getInstance());
    if (!valueDataInstance) {
      LM_X(1, ("Internal error:"));
    }
    valueDataInstances_.push_back(valueDataInstance);
  }

  // Alloc necessary space for KVSetStruct ( used in 3rd party interface )
  inputStructs_ = reinterpret_cast<KVSetStruct *>(malloc(sizeof(KVSetStruct) * num_inputs_));
}

KVInputVector::KVInputVector(int num_inputs) {
  num_inputs_ = num_inputs;

  _kv = NULL;
  kv = NULL;

  max_num_kvs = 0;   // Maximum number of kvs to be processes
  num_kvs = 0;   // Current number of key-values pairs

  inputStructs_ = NULL;

  for (size_t i = 0; i < valueDataInstances_.size(); i++) {
    delete valueDataInstances_[i];
  }
  valueDataInstances_.clear();
}

KVInputVector::~KVInputVector() {
  if (_kv) {
    free(_kv);
  }
  if (kv) {
    free(kv);
  }
  if (inputStructs_) {
    free(inputStructs_);
  }
}

void KVInputVector::addKVs(int input, KVInfo info, KV *kvs) {
  for (size_t i = 0; i < info.kvs; i++) {
    kv[num_kvs] = kvs[i];
    kv[num_kvs].input = input;
    num_kvs++;
  }
}

void KVInputVector::addKVs(int input, KVInfo info, char *data) {
  if (input >= static_cast<int>(valueDataInstances_.size())) {
    LOG_SW(("Error adding key-values to a KVInputVector. Ignoring..."));
    return;
  }

  // Get the right data instance
  DataInstance *valueDataInstance = valueDataInstances_[input];

  // Local offset
  size_t offset = 0;

  // Process a set of key values
  for (size_t i = 0; i < info.kvs; i++) {
    kv[num_kvs].key = data + offset;

    kv[num_kvs].key_size = keyDataInstance_->parse(data + offset);
    offset += kv[num_kvs].key_size;

    kv[num_kvs].value = data + offset;

    kv[num_kvs].value_size = valueDataInstance->parse(data + offset);
    offset += kv[num_kvs].value_size;

    kv[num_kvs].input = input;

    kv[num_kvs].pos = num_kvs;

    num_kvs++;
  }

  // Make sure the parsing is OK!
  if (offset != info.size) {
    LM_X(1,
         (
           "Error adding key-values to a KVInputVector for input %d (%s). (Offset %lu != info.size %lu) KVS num_kvs:%lu / max_num_kvs:%lu ",
           input, info.str().c_str(), offset, info.size, num_kvs, max_num_kvs));
  }
}

std::string str_kv(KV *kv) {
  std::ostringstream output;

  output << au::str("KV[%d][%d]", kv->key_size, kv->value_size);
  return output.str();
}

bool equalKV(KV *kv1, KV *kv2) {
  if (kv1->key_size != kv2->key_size) {
    return false;
  }
  for (int i = 0; i < kv1->key_size; i++) {
    if (kv1->key[i] != kv2->key[i]) {
      return false;
    }
  }

  return true;
}

bool compareKV(KV *kv1, KV *kv2) {
  if (kv1->key_size < kv2->key_size) {
    return true;
  }

  if (kv1->key_size > kv2->key_size) {
    return false;
  }

  for (int i = 0; i < kv1->key_size; i++) {
    if (kv1->key[i] < kv2->key[i]) {
      return true;
    }

    if (kv1->key[i] > kv2->key[i]) {
      return false;
    }
  }

  // Compare by input
  if (kv1->input != kv2->input) {
    return kv1->input < kv2->input;
  }

  // Same key!

  if (kv1->value_size < kv2->value_size) {
    return true;
  }

  if (kv1->value_size > kv2->value_size) {
    return false;
  }

  for (int i = 0; i < kv1->value_size; i++) {
    if (kv1->value[i] < kv2->value[i]) {
      return true;
    }

    if (kv1->value[i] > kv2->value[i]) {
      return false;
    }
  }

  // Same key and value!

  // If they are the same....
  return false;
}

// global sort function key - input - value used in reduce operations

void KVInputVector::sort() {
  if (num_kvs > 0) {
    std::sort(_kv, _kv + num_kvs, compareKV);
  }
}

void KVInputVector::sortAndMerge(size_t middle_pos) {
  if (middle_pos > num_kvs) {
    LM_X(1, ("Internal error"));   // Sort the first part of the vector
  }
  std::sort(_kv, _kv + middle_pos, compareKV);

  // Merge with the second part of the vector( supposed to be sorted )
  std::inplace_merge(_kv, _kv + middle_pos, _kv + num_kvs, compareKV);
}

void KVInputVector::Init() {
  // Process all the key-values in order
  pos_begin_ = 0;   // Position where the next group of key-values begin
  pos_end_ = 1;   // Position where the next group of key-values finish
}

KVSetStruct *KVInputVector::GetNext() {
  if (pos_begin_ >= num_kvs) {
    return NULL;
  }

  // Identify the number of key-values with the same key
  while ((pos_end_ < num_kvs) && equalKV(_kv[pos_begin_], _kv[pos_end_])) {
    pos_end_++;
  }

  // Create the necessary elements for the output KVSetStruct structure
  size_t pos_pointer = pos_begin_;
  for (int i = 0; i < num_inputs_; ++i) {
    if ((pos_pointer == pos_end_) || (_kv[pos_pointer]->input != i)) {
      inputStructs_[i].num_kvs = 0;
    } else {
      inputStructs_[i].kvs = &_kv[pos_pointer];
      inputStructs_[i].num_kvs = 0;
      while ((pos_pointer < pos_end_) && (_kv[pos_pointer]->input == i)) {
        inputStructs_[i].num_kvs++;
        pos_pointer++;
      }
    }
  }

  // Go to the next position
  pos_begin_ = pos_end_;
  pos_end_ = pos_begin_ + 1;

  return inputStructs_;
}
}
