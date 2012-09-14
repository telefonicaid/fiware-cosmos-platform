#include "samson_system/ValueReduce.h"

#include <stdlib.h>   // malloc()

namespace samson {
namespace system {

ValueList::ValueList(int max_num_elements) : max_num_elements_(max_num_elements)
    , values_(static_cast<Value**>(malloc(max_num_elements_ * sizeof(*values_))))
    , counters_(static_cast<int *>(malloc(max_num_elements_ * sizeof(*counters_)))) {
  for (int i = 0; i < max_num_elements_; i++) {
    values_[i] = new Value();
    counters_[i] = 0;             // Init counter to 0
  }
}

ValueList::~ValueList() {
  for (int i = 0; i < max_num_elements_; i++) {
    delete values_[i];
  }
  free(values_);
  free(counters_);
}

void ValueList::Push(Value *value, int counter) {
  for (int i = 0; i < max_num_elements_; ++i) {
    if (counter > counters_[i]) {
      // Take the last and get the content
      Value *tmp = values_[max_num_elements_ - 1];
      tmp->copyFrom(value);

      // Move the rest of pointers and counter
      for (int j = (max_num_elements_ - 2); j >= i; --j) {
        values_[j + 1] = values_[j];
        counters_[j + 1] = counters_[j];
      }

      // Insert the new one
      values_[i] = tmp;
      counters_[i] = counter;

      break;
    }
  }
}

void ValueList::SetTops(Value *value) {
  // Special case for 1 top element to not create double linked vector [[concept num]]
  if (max_num_elements_ == 1) {
    if (counters_[0] == 0) {
      value->SetAsVoid();
    } else {
      value->SetAsVector();
      value->AddValueToVector()->copyFrom(values_[0]);
      value->AddValueToVector()->SetDouble(counters_[0]);
    }
  }

  value->SetAsVector();
  for (int i = 0; i < max_num_elements_; ++i) {
    if (counters_[i] > 0) {
      Value *tmp = value->AddValueToVector();

      tmp->SetAsVector();
      tmp->AddValueToVector()->copyFrom(values_[i]);
      tmp->AddValueToVector()->SetDouble(counters_[i]);
    }
  }
}

void ValueReduce_top::Init(std::string command) {
  au::CommandLine cmdLine;

  cmdLine.Parse(command);

  int num = 1;
  if (cmdLine.get_num_arguments() > 1) {
    num = atoi(cmdLine.get_argument(1).c_str());
  }
  if (num <= 0) {
    num = 1;
  }

  list_ = new ValueList(num);
}

void ValueReduce_top::Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
  // Init the list
  list_->Init();

  // Parse common key
  key_.parse(inputs[0].kvs[0]->key);

  // Parse all the values ( grouping by
  size_t pos = 0;
  while (pos < inputs[0].num_kvs) {
    value_.parse(inputs[0].kvs[pos]->value);
    int counter = inputs[0].kvs_with_equal_value(pos);

    // Push to the list
    list_->Push(&value_, counter);

    // Skip this value
    pos += counter;
  }

  // Prepare and emit output
  list_->SetTops(&value_);
  writer->emit(0, &key_, &value_);
  return;
}


void ValueReduce_top_concept::Init(std::string command) {
  au::CommandLine cmdLine;

  cmdLine.Parse(command);

  int num = 1;
  if (cmdLine.get_num_arguments() > 1) {
    num = atoi(cmdLine.get_argument(1).c_str());
  }
  if (num == 0) {
    num = 1;
  }

  list_ = new ValueList(num);
}

void ValueReduce_top_concept::Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
  // Init the list
  list_->Init();

  // Parse common key
  key_.parse(inputs[0].kvs[0]->key);

  // Parse all the values ( grouping by
  for (size_t i = 0; i < inputs[0].num_kvs; ++i) {
    // Parse the value
    value_.parse(inputs[0].kvs[i]->value);

    if (!value_.IsVector()) {
      continue;
    }
    if (value_.GetVectorSize() != 2) {
      continue;
    }

    int counter = value_.GetValueFromVector(1)->GetDouble();

    // Push to the list
    list_->Push(value_.GetValueFromVector(0), counter);
  }

  // Prepare and emit output
  list_->SetTops(&value_);
  writer->emit(0, &key_, &value_);
  return;
}


void ValueReduce_update_sum::Init(std::string command) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagBoolean("emit");
  cmdLine.SetFlagInt("time", 0);
  cmdLine.Parse(command);

  time_span_ = cmdLine.GetFlagInt("time");
  emit_ = cmdLine.GetFlagBool("emit");

  if (time_span_ == 0) {
    factor_ = 1.0;
  } else {
    factor_ = static_cast<double>(time_span_ - 1) / static_cast<double>(time_span_);
  }
}

void ValueReduce_update_sum::Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
  // Parse common key
  if (inputs[0].num_kvs > 0) {
    key_.parse(inputs[0].kvs[0]->key);
  } else {
    key_.parse(inputs[1].kvs[0]->key);
  }

  // Recover state if any

  double total = 0;
  time_t t = ::time(NULL);

  if (inputs[1].num_kvs > 0) {
    value_.parse(inputs[1].kvs[0]->value);

    if (value_.IsVector() && (value_.GetVectorSize() == 2)) {
      // Recover previous
      total = value_.GetValueFromVector(0)->GetDouble();

      // Forgetting factor
      size_t time_diff = t - value_.GetValueFromVector(1)->GetDouble();
      total = total * pow(factor_, static_cast<int> (time_diff));
    }

    double tmp = value_.GetDouble();
    total += tmp;
  }

  for (size_t i = 0; i < inputs[0].num_kvs; ++i) {
    value_.parse(inputs[0].kvs[i]->value);
    double tmp = value_.GetDouble();
    total += tmp;
  }

  // Create the output value as a vector [ total time ]
  value_.SetAsVector();
  value_.AddValueToVector()->SetDouble(total);
  value_.AddValueToVector()->SetDouble(t);

  // Emit to update the state
  writer->emit(1, &key_, &value_);

  // Emit to output if necessary
  if (emit_) {
    writer->emit(0, &key_, &value_);
  }

  return;
}


void ValueReduce_accumulate::Init(std::string command) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagInt("time", 360);
  cmdLine.Parse(command);

  time_span_ = cmdLine.GetFlagInt("time");

  if (time_span_ == 0) {
    factor_ = 1.0;
  } else {
    factor_ = static_cast<double>(time_span_ - 1.0) / static_cast<double>(time_span_);
  }
  current_time_ = ::time(NULL);
}

void ValueReduce_accumulate::UpdateCounter(system::Value *value) {
  // Forgetting factor
  size_t time_diff = current_time_ - value->GetValueFromMap("timestamp")->GetDouble();
  system::Value *counter_value = value->GetValueFromMap("counter");

  double previous_counter = counter_value->GetDouble();
  double counter = previous_counter * pow(factor_, static_cast<int> (time_diff));

  counter_value->SetDouble(counter);
}

void ValueReduce_accumulate::Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
  // Parse common key
  // ------------------------------------

  if (inputs[0].num_kvs > 0) {
    key_.parse(inputs[0].kvs[0]->key);
  } else {
    key_.parse(inputs[1].kvs[0]->key);
  }
  // Check valid key
  if (key_.GetValueFromMap("category") == NULL) {
    return;
  }
  if (key_.GetValueFromMap("concept") == NULL) {
    return;
  }
  // Recover state if any
  // ------------------------------------

  double total = 0;
  time_t t = time(NULL);

  if (inputs[1].num_kvs > 0) {
    value_.parse(inputs[1].kvs[0]->value);
    UpdateCounter(&value_);

    // Counter to accumulate inputs
    total = value_.GetValueFromMap("counter")->GetDouble();
  } else {
    value_.SetAsMap();
    // Copy value and category from key
    value_.AddValueToMap("category")->copyFrom(key_.GetValueFromMap("category"));
    value_.AddValueToMap("concept")->copyFrom(key_.GetValueFromMap("concept"));

    // Add time-stamp and counter
    value_.AddValueToMap("timestamp")->SetDouble(t);
    value_.AddValueToMap("counter")->SetDouble(0);
    total = 0;
  }

  // Compute the new total
  for (size_t i = 0; i < inputs[0].num_kvs; ++i) {
    tmp_value_.parse(inputs[0].kvs[i]->value);
    if (tmp_value_.IsNumber()) {
      double tmp = tmp_value_.GetDouble();
      total += tmp;
    }
  }

  // Emit to update the state
  value_.AddValueToMap("counter")->SetDouble(total);
  writer->emit(1, &key_, &value_);

  // Emit to accumulate by category
  key_.SetString(value_.GetValueFromMap("category")->GetString());
  writer->emit(0, &key_, &value_);

  return;
}


void ValueReduce_accumulate_top::Init(std::string command) {
  au::CommandLine cmdLine;

  cmdLine.SetFlagUint64("time_span", 360);
  cmdLine.Parse(command);

  time_span_ = cmdLine.GetFlagInt("time");

  if (time_span_ == 0) {
    factor_ = 1.0;
  } else {
    factor_ = static_cast<double>(time_span_ - 1.0) / static_cast<double>(time_span_);
  }
  current_time_ = ::time(NULL);
}

void ValueReduce_accumulate_top::UpdateCounter(system::Value *value) {
  // Forgetting factor
  size_t time_diff = current_time_ - value->GetValueFromMap("timestamp")->GetDouble();
  system::Value *counter_value = value->GetValueFromMap("counter");

  counter_value->SetDouble(counter_value->GetDouble() * pow(factor_, static_cast<int> (time_diff)));
}

void ValueReduce_accumulate_top::Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
  // Parse common key
  if (inputs[0].num_kvs > 0) {
    key_.parse(inputs[0].kvs[0]->key);
  } else {
    key_.parse(inputs[1].kvs[0]->key);
  }

  // Recover elements in the state
  if (inputs[1].num_kvs > 0) {
    value_.parse(inputs[1].kvs[0]->value);

    size_t num_elements = value_.GetVectorSize();
    for (size_t i = 0; i < num_elements; ++i) {
      system::Value *v = value_.GetValueFromVector(i);
      UpdateCounter(v);
      //        top_list_.Push(v);
    }
  }

  // Process all inputs ( if any )
  for (size_t i = 0; i < inputs[0].num_kvs; ++i) {
    value_.parse(inputs[0].kvs[i]->value);
    top_list_.Push(&value_);
  }

  // Emit to update the state
  top_list_.Get(&value_);

  // Emit state and output
  writer->emit(0, &key_, &value_);
  writer->emit(1, &key_, &value_);

  return;
}




}  // namespaces
}
