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
#ifndef _H_SAMSON_system_reduce_VALUE_REDUCE
#define _H_SAMSON_system_reduce_VALUE_REDUCE

#include <list>
#include <map>
#include <string>
#include <utility>          // pair<>

#include "au/containers/list.h"
#include "samson/module/samson.h"
#include "samson_system/Value.h"

namespace samson {
namespace system {
// ---------------------------------------------
// ValueList
// ---------------------------------------------

class ValueList {
  public:
    explicit ValueList(int _max_num_elements);
    ~ValueList();

    void Init() {
      for (int i = 0; i < max_num_elements_; ++i) {
        counters_[i] = 0;   // Init counter to 0
      }
    }

    void Push(Value *value, int counter);
    void SetTops(Value *value);

  private:
    int max_num_elements_;
    // List of values
    Value **values_;
    int *counters_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduce
//
//       Generic operation for a reduce operation
// -----------------------------------------------------------------------------------------------

class ValueReduce {
  public:
    ValueReduce() {}

    virtual ~ValueReduce() {}

    virtual void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) = 0;
    virtual void Init(std::string command) {}  // Optional method to receive extra parameters
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_all
//
//       Create a vector with all values for each key
// -----------------------------------------------------------------------------------------------

class ValueReduce_all : public ValueReduce {
  public:
    ValueReduce_all() : ValueReduce()
    , key_()
    , value_() {}

    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
      // Parse common key
      key_.parse(inputs[0].kvs[0]->key);

      value_.SetAsVector();
      for (size_t i = 0; i < inputs[0].num_kvs; ++i) {
        value_.AddValueToVector()->parse(inputs[0].kvs[i]->value);
      }

      writer->emit(0, &key_, &value_);
      return;
    }

  private:
    samson::system::Value key_;
    samson::system::Value value_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_unique
//
//       Create a vector with all unique values for each key
// -----------------------------------------------------------------------------------------------

class ValueReduce_unique : public ValueReduce {
  public:
    ValueReduce_unique() : ValueReduce()
    , key_()
    , value_()
    , output_value_() {}

    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
      // Parse common key
      key_.parse(inputs[0].kvs[0]->key);

      output_value_.SetAsVector();

      size_t pos = 0;
      while (pos < inputs[0].num_kvs) {
        value_.parse(inputs[0].kvs[pos]->value);
        int counter = inputs[0].kvs_with_equal_value(pos);

        output_value_.AddValueToVector()->copyFrom(&value_);

        pos += counter;
      }

      writer->emit(0, &key_, &output_value_);
      return;
    }

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    samson::system::Value output_value_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_unique_counter
//
//       Create a vector with all unique values for each key and an individual counter for each one
// -----------------------------------------------------------------------------------------------

class ValueReduce_unique_counter : public ValueReduce {
  public:
    ValueReduce_unique_counter() : ValueReduce()
    , key_()
    , value_()
    , output_value_() {}

    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
      // Parse common key
      key_.parse(inputs[0].kvs[0]->key);

      output_value_.SetAsVector();

      size_t pos = 0;
      while (pos < inputs[0].num_kvs) {
        value_.parse(inputs[0].kvs[pos]->value);
        int counter = inputs[0].kvs_with_equal_value(pos);

        samson::system::Value *new_value = output_value_.AddValueToVector();

        new_value->SetAsVector();
        new_value->AddValueToVector()->copyFrom(&value_);
        new_value->AddValueToVector()->SetDouble(counter);

        pos += counter;
      }

      writer->emit(0, &key_, &output_value_);
      return;
    }

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    samson::system::Value output_value_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_top
//
//       Emit only the most popular value
// -----------------------------------------------------------------------------------------------

class ValueReduce_top : public ValueReduce {
  public:
    ValueReduce_top() :
      list_(NULL) {}

    ~ValueReduce_top() {
      if (list_) {
        delete list_;
      }
    }

    void Init(std::string command);
    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer);

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    ValueList *list_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_top_concept
//
//       Emit only the most popular value
// -----------------------------------------------------------------------------------------------

class ValueReduce_top_concept : public ValueReduce {
  public:
    ValueReduce_top_concept() : list_(NULL) {}

    ~ValueReduce_top_concept() {
      if (list_) {
        delete list_;
      }
    }

    void Init(std::string command);
    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer);

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    ValueList *list_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_sum
//
//       Emit the sum of values ( assuming numbers )
// -----------------------------------------------------------------------------------------------


class ValueReduce_sum : public ValueReduce {
  public:
    ValueReduce_sum() : ValueReduce()
    , key_()
    , value_()
    , output_value_() {}

    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
      // Parse common key
      key_.parse(inputs[0].kvs[0]->key);

      double total = 0;
      for (size_t i = 0; i < inputs[0].num_kvs; ++i) {
        value_.parse(inputs[0].kvs[i]->value);
        double tmp = value_.GetDouble();
        total += tmp;
      }

      value_ = total;
      writer->emit(0, &key_, &value_);
      return;
    }

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    samson::system::Value output_value_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_average
//
//       Emit the average of values ( assuming numbers )
// -----------------------------------------------------------------------------------------------


class ValueReduce_average : public ValueReduce {
  public:
    ValueReduce_average() : ValueReduce()
    , key_()
    , value_()
    , output_value_() {}

    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
      // Parse common key
      key_.parse(inputs[0].kvs[0]->key);

      double total = 0;
      for (size_t i = 0; i < inputs[0].num_kvs; ++i) {
        value_.parse(inputs[0].kvs[i]->value);
        double tmp = value_.GetDouble();
        total += tmp;
      }
      total /= static_cast<double> (inputs[0].num_kvs);
      value_ = total;
      writer->emit(0, &key_, &value_);
      return;
    }

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    samson::system::Value output_value_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_update_sum
//
//       Emit the average of values ( assuming numbers )
// -----------------------------------------------------------------------------------------------


class ValueReduce_update_sum : public ValueReduce {
  public:
    ValueReduce_update_sum() : ValueReduce()
    , key_()
    , value_()
    , output_value_()
    , time_span_(300)
    , factor_((time_span_ - 1) / time_span_) {}

    void Init(std::string command);
    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer);

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    samson::system::Value output_value_;
    int time_span_;
    bool emit_;
    double factor_;   // Forgetting factor
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_update_last
//
//       Emit the average of values ( assuming numbers )
// -----------------------------------------------------------------------------------------------


class ValueReduce_update_last : public ValueReduce {
  public:
    ValueReduce_update_last() : key_()
    , value_()
    , output_value_() {}

    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer) {
      // If inputs, emit the last input
      if (inputs[1].num_kvs > 0) {
        size_t num_kvs = inputs[1].num_kvs;
        key_.parse(inputs[1].kvs[num_kvs - 1]->key);
        value_.parse(inputs[1].kvs[num_kvs - 1]->key);
        writer->emit(0, &key_, &value_);
        return;
      }

      if (inputs[0].num_kvs > 0) {
        size_t num_kvs = inputs[0].num_kvs;
        key_.parse(inputs[0].kvs[num_kvs - 1]->key);
        value_.parse(inputs[0].kvs[num_kvs - 1]->key);
        writer->emit(0, &key_, &value_);
        return;
      }
    }

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    samson::system::Value output_value_;
};

// Accumulate elements {... counter=# ... }
class TopList {
  public:
    TopList() :
      num_elements_(100) {}

    ~TopList() {
      // Remove all newly created instances
      values_.clearList();
    }

    void Push(Value *value) {
      double new_num = value->GetValueFromMap(Value::kCounterField.c_str())->GetDouble();

      // Push a new element
      std::list<system::Value *>::iterator it_values;
      for (it_values = values_.begin(); it_values != values_.end(); ++it_values) {
        double num = (*it_values)->GetValueFromMap(Value::kCounterField.c_str())->GetDouble();
        if (new_num > num) {
          values_.insert(it_values, Copy(value));
          return;
        }
      }

      if (num_elements_ > values_.size()) {
        values_.push_back(Copy(value));
      }
    }

    void Get(system::Value *output_value) {
      output_value->SetAsVector();
      std::list<system::Value *>::iterator it_values;
      for (it_values = values_.begin(); it_values != values_.end(); ++it_values) {
        output_value->AddValueToVector()->copyFrom(*it_values);
      }
    }

    size_t num_elements_;   // Maximum number of elements
    au::list<system::Value> values_;   // List of values

  private:
    system::Value *Copy(system::Value *v) {
      system::Value *c = new system::Value();
      c->copyFrom(v);
      return c;
    }
};

// -----------------------------------------------------------------------------------------------
//
// ValueReduce_accumulate
//
// Received     { value:"XX" category:"XX" } , counter
// State        { value:"XX" category:"XX" } , { value:"XX" category:"XX" counter:# timestamp:# }
// Emit         category                     , { value:"XX" category:"XX" counter:# timestamp:# }
// -----------------------------------------------------------------------------------------------

// TODO: Emit only relevant outputs after processing all input key-values

class ValueReduce_accumulate : public ValueReduce {
  public:
    ValueReduce_accumulate() : ValueReduce()
    , key_()
    , value_()
    , tmp_value_()
    , output_value_()
    , time_span_(300)
    , factor_((time_span_ - 1) / time_span_)
    , current_time_(0) {}

    void Init(std::string command);
    void UpdateCounter(system::Value *value);
    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer);

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    samson::system::Value tmp_value_;
    samson::system::Value output_value_;
    int time_span_;
    double factor_;   // Forgetting factor
    size_t current_time_;
};

// -----------------------------------------------------------------------------------------------
//
// ValueReduce_accumulate_top
//
// Received     category                     , { value:"XX" category:"XX" counter:# timestamp:# }
// State        category                     , [ { value:"XX" counter:# timestamp:# } ... ]
// Emit         category                     , [ { value:"XX" counter:# timestamp:# } ... ]
// -----------------------------------------------------------------------------------------------

// TODO: Emit only relevant outputs after processing all input key-values

class ValueReduce_accumulate_top : public ValueReduce {
  public:
    ValueReduce_accumulate_top() : ValueReduce()
    , key_()
    , value_()
    , output_value_()
    , time_span_(300)
    , factor_((time_span_ - 1) / time_span_)
    , top_list_()
    , current_time_(0) {}

    void Init(std::string command);
    void UpdateCounter(system::Value *value);
    void Run(samson::KVSetStruct *inputs, samson::KVWriter* const writer);

  private:
    samson::system::Value key_;
    samson::system::Value value_;
    samson::system::Value output_value_;
    int time_span_;
    double factor_;     // Forgetting factor
    TopList top_list_;  // List to keep top elements
    size_t current_time_;
};

// -----------------------------------------------------------------------------------------------
// ValueReduceManager manager
//
//     Factory of ValueReduce operations
// -----------------------------------------------------------------------------------------------

typedef ValueReduce *(*factory_ValueReduce)();

template<class C>
ValueReduce *factory_ValueReduce_impl() {
  return new C();
}

class ValueReduceManager {
  public:
    explicit ValueReduceManager(std::string type) {
      if (type == "reduce") {
        Add<ValueReduce_all> ("all");
        Add<ValueReduce_unique> ("unique");
        Add<ValueReduce_unique_counter> ("unique_counter");

        Add<ValueReduce_top> ("top");                  // Expect key - value
        Add<ValueReduce_top_concept> ("top_concept");  // Expect key - [ value counter ]

        Add<ValueReduce_sum> ("sum");
        Add<ValueReduce_average> ("average");
      }

      if (type == "update") {
        Add<ValueReduce_update_last> ("last");   // Keep the last value
        Add<ValueReduce_update_sum> ("sum");     // Keep the total sum of seen values

        Add<ValueReduce_accumulate> ("accumulate");
        // Received     { value:"XX" category:"XX" } , counter
        // State        { value:"XX" category:"XX" } , { value:"XX" category:"XX" counter:# timestamp:# }
        // Emit         category                     , { value:"XX" category:"XX" counter:# timestamp:# }

        Add<ValueReduce_accumulate_top> ("accumulate_top");
        // Received     category                     , { value:"XX" category:"XX" counter:# timestamp:# }
        // State        category                     , [ { value:"XX" counter:# timestamp:# } ... ]
        // Emit         category                     , [ { value:"XX" counter:# timestamp:# } ... ]
      }
    }

    template<class C>
    void Add(std::string name) {
      factories_.insert(std::pair<std::string, factory_ValueReduce>(name, factory_ValueReduce_impl<C>));
    }

    ValueReduce *getInstance(std::string command) {
      // Discover the main command
      au::CommandLine cmdLine;

      cmdLine.Parse(command);

      if (cmdLine.get_num_arguments() == 0) {
        return NULL;
      }

      // Main command
      std::string name = cmdLine.get_argument(0);

      std::map<std::string, factory_ValueReduce>::iterator it_factories;
      it_factories = factories_.find(name);

      if (it_factories == factories_.end()) {
        return NULL;
      } else {
        ValueReduce *value_reduce = it_factories->second();
        value_reduce->Init(command);
        return value_reduce;
      }
    }

    std::string GetListOfCommands() {
      std::ostringstream output;

      std::map<std::string, factory_ValueReduce>::iterator it_factories;
      for (it_factories = factories_.begin(); it_factories != factories_.end(); ++it_factories) {
        output << it_factories->first << " ";
      }
      return output.str();
    }

  private:
    // Map with all available commands
    std::map<std::string, factory_ValueReduce> factories_;
};
}
}

#endif  // ifndef _H_SAMSON_system_reduce_VALUE_REDUCE
