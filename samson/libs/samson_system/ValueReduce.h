
#ifndef _H_SAMSON_system_reduce_VALUE_REDUCE
#define _H_SAMSON_system_reduce_VALUE_REDUCE


#include "au/containers/list.h"

#include <samson/module/samson.h>
#include <samson_system/Value.h>


namespace samson {
namespace system {
// ---------------------------------------------
// ValueList
// ---------------------------------------------

class ValueList {
  int max_num_elements;

  // List of values
  Value **values;
  int *counters;

public:

  ValueList(int _max_num_elements);

  void init() {
    for (int i = 0; i < max_num_elements; i++) {
      counters[i] = 0;               // Init counter to 0
    }
  }

  void push(Value *value, int counter) {
    for (int i = 0; i < max_num_elements; i++) {
      if (counter > counters[i]) {
        // Take the last and get the content
        Value *tmp = values[ max_num_elements - 1 ];
        tmp->copyFrom(value);

        // Move the rest of pointers and counter
        for (int j = (max_num_elements - 2 ); j >= i; j--) {
          values[j + 1] = values[j];
          counters[j + 1] = counters[j];
        }

        // Inser the new one
        values[i] = tmp;
        counters[i] = counter;

        break;
      }
    }
  }

  void set_tops(Value *value) {
    // Spetial case for 1 top element to not create double linked vector [[concept num]]
    if (max_num_elements == 1) {
      if (counters[0] == 0) {
        value->set_as_void();
      } else {
        value->set_as_vector();
        value->add_value_to_vector()->copyFrom(values[0]);
        value->add_value_to_vector()->set_double(counters[0]);
      }
    }

    value->set_as_vector();
    for (int i = 0; i < max_num_elements; i++) {
      if (counters[i] > 0) {
        Value *tmp = value->add_value_to_vector();

        tmp->set_as_vector();
        tmp->add_value_to_vector()->copyFrom(values[i]);
        tmp->add_value_to_vector()->set_double(counters[i]);
      }
    }
  }
};

// -----------------------------------------------------------------------------------------------
// ValueReduce
//
//       Generic operation for a reduce operation
// -----------------------------------------------------------------------------------------------

class ValueReduce {
public:

  virtual void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) = 0;
  virtual void init(std::string command) {
    // Optional method to receive extra parameters
  }
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_all
//
//       Create a vector with all values for each key
// -----------------------------------------------------------------------------------------------

class ValueReduce_all : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

public:

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Parse common key
    key.parse(inputs[0].kvs[0]->key);

    value.set_as_vector();
    for (size_t i = 0; i < inputs[0].num_kvs; i++) {
      value.add_value_to_vector()->parse(inputs[0].kvs[i]->value);
    }

    writer->emit(0, &key, &value);
    return;
  }
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_unique
//
//       Create a vector with all unique values for each key
// -----------------------------------------------------------------------------------------------

class ValueReduce_unique : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

  samson::system::Value output_value;

public:

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Parse common key
    key.parse(inputs[0].kvs[0]->key);

    output_value.set_as_vector();

    size_t pos = 0;
    while (pos < inputs[0].num_kvs) {
      value.parse(inputs[0].kvs[pos]->value);
      int counter = inputs[0].kvs_with_equal_value(pos);

      output_value.add_value_to_vector()->copyFrom(&value);

      pos += counter;
    }

    writer->emit(0, &key, &output_value);
    return;
  }
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_unique_counter
//
//       Create a vector with all unique values for each key and an individual counter for each one
// -----------------------------------------------------------------------------------------------

class ValueReduce_unique_counter : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

  samson::system::Value output_value;

public:

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Parse common key
    key.parse(inputs[0].kvs[0]->key);

    output_value.set_as_vector();


    size_t pos = 0;
    while (pos < inputs[0].num_kvs) {
      value.parse(inputs[0].kvs[pos]->value);
      int counter = inputs[0].kvs_with_equal_value(pos);

      samson::system::Value *new_value = output_value.add_value_to_vector();

      new_value->set_as_vector();
      new_value->add_value_to_vector()->copyFrom(&value);
      new_value->add_value_to_vector()->set_double(counter);

      pos += counter;
    }

    writer->emit(0, &key, &output_value);
    return;
  }
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_top
//
//       Emit only the most popular value
// -----------------------------------------------------------------------------------------------

class ValueReduce_top : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

  ValueList *list;

public:


  ValueReduce_top() {
    list = NULL;
  }

  ~ValueReduce_top() {
    if (list)
      delete list;
  }

  void init(std::string command) {
    au::CommandLine cmdLine;

    cmdLine.parse(command);

    int num = 1;
    if (cmdLine.get_num_arguments() > 1)
      num = atoi(cmdLine.get_argument(1).c_str());
    if (num <= 0)
      num = 1;

    list = new ValueList(num);
  }

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Init the list
    list->init();

    // Parse common key
    key.parse(inputs[0].kvs[0]->key);

    // Parse all the values ( grouping by
    size_t pos = 0;
    while (pos < inputs[0].num_kvs) {
      value.parse(inputs[0].kvs[pos]->value);
      int counter = inputs[0].kvs_with_equal_value(pos);

      // Push to the list
      list->push(&value, counter);

      // Skip this value
      pos += counter;
    }

    // Prepare and emit output
    list->set_tops(&value);
    writer->emit(0, &key, &value);
    return;
  }
};


// -----------------------------------------------------------------------------------------------
// ValueReduce_top_concept
//
//       Emit only the most popular value
// -----------------------------------------------------------------------------------------------

class ValueReduce_top_concept : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

  ValueList *list;

public:


  ValueReduce_top_concept() {
    list = NULL;
  }

  ~ValueReduce_top_concept() {
    if (list)
      delete list;
  }

  void init(std::string command) {
    au::CommandLine cmdLine;

    cmdLine.parse(command);

    int num = 1;
    if (cmdLine.get_num_arguments() > 1)
      num = atoi(cmdLine.get_argument(1).c_str());
    if (num == 0)
      num = 1;

    list = new ValueList(num);
  }

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Init the list
    list->init();

    // Parse common key
    key.parse(inputs[0].kvs[0]->key);

    // Parse all the values ( grouping by
    for (size_t i = 0; i < inputs[0].num_kvs; i++) {
      // Parse the value
      value.parse(inputs[0].kvs[i]->value);

      if (!value.isVector())
        continue;
      if (value.get_vector_size() != 2)
        continue;

      int counter = value.get_value_from_vector(1)->get_double();

      // Push to the list
      list->push(value.get_value_from_vector(0), counter);
    }

    // Prepare and emit output
    list->set_tops(&value);
    writer->emit(0, &key, &value);
    return;
  }
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_sum
//
//       Emit the sum of values ( assuming numbers )
// -----------------------------------------------------------------------------------------------


class ValueReduce_sum : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

  samson::system::Value output_value;

public:

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Parse common key
    key.parse(inputs[0].kvs[0]->key);

    double total = 0;
    for (size_t i = 0; i < inputs[0].num_kvs; i++) {
      value.parse(inputs[0].kvs[i]->value);
      double tmp =  value.get_double();
      total += tmp;
    }

    value = total;
    writer->emit(0, &key, &value);
    return;
  }
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_average
//
//       Emit the average of values ( assuming numbers )
// -----------------------------------------------------------------------------------------------


class ValueReduce_average : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

  samson::system::Value output_value;

public:

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Parse common key
    key.parse(inputs[0].kvs[0]->key);

    double total = 0;
    for (size_t i = 0; i < inputs[0].num_kvs; i++) {
      value.parse(inputs[0].kvs[i]->value);
      double tmp =  value.get_double();
      total += tmp;
    }
    total /= (double)inputs[0].num_kvs;
    value = total;
    writer->emit(0, &key, &value);
    return;
  }
};



// -----------------------------------------------------------------------------------------------
// ValueReduce_update_sum
//
//       Emit the average of values ( assuming numbers )
// -----------------------------------------------------------------------------------------------


class ValueReduce_update_sum : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

  samson::system::Value output_value;

  int time_span;
  bool emit;

  double factor;           // Forgetting facto

public:

  void init(std::string command) {
    au::CommandLine cmdLine;

    cmdLine.set_flag_boolean("emit");
    cmdLine.set_flag_int("time", 0);
    cmdLine.parse(command);

    time_span = cmdLine.get_flag_int("time");
    emit = cmdLine.get_flag_bool("emit");

    if (time_span == 0)
      factor = 1;
    else
      factor = ((double)time_span - 1 ) / (double)time_span;
  }

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Parse common key
    if (inputs[0].num_kvs > 0)
      key.parse(inputs[0].kvs[0]->key);
    else
      key.parse(inputs[1].kvs[0]->key);

    // Recover state if any

    double total = 0;
    time_t t = time(NULL);

    if (inputs[1].num_kvs > 0) {
      value.parse(inputs[1].kvs[0]->value);

      if (value.isVector() && (value.get_vector_size() == 2)) {
        // Recover previous
        total = value.get_value_from_vector(0)->get_double();

        // Forgetting factor
        size_t time_diff = t - value.get_value_from_vector(1)->get_double();
        total = total * pow(factor, time_diff);
      }

      double tmp =  value.get_double();
      total += tmp;
    }

    for (size_t i = 0; i < inputs[0].num_kvs; i++) {
      value.parse(inputs[0].kvs[i]->value);
      double tmp =  value.get_double();
      total += tmp;
    }


    // Create the output value as a vector [ total time ]
    value.set_as_vector();
    value.add_value_to_vector()->set_double(total);
    value.add_value_to_vector()->set_double(t);

    // Emit to update the state
    writer->emit(1, &key, &value);

    // Emit to output if necessary
    if (emit)
      writer->emit(0, &key, &value);

    return;
  }
};

// -----------------------------------------------------------------------------------------------
// ValueReduce_update_last
//
//       Emit the average of values ( assuming numbers )
// -----------------------------------------------------------------------------------------------


class ValueReduce_update_last : public ValueReduce {
  samson::system::Value key;
  samson::system::Value value;

  samson::system::Value output_value;

public:

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // If inputs, emit the last input
    if (inputs[1].num_kvs > 0) {
      size_t num_kvs = inputs[1].num_kvs;
      key.parse(inputs[1].kvs[num_kvs - 1]->key);
      value.parse(inputs[1].kvs[num_kvs - 1]->key);
      writer->emit(0, &key, &value);
      return;
    }

    if (inputs[0].num_kvs > 0) {
      size_t num_kvs = inputs[0].num_kvs;
      key.parse(inputs[0].kvs[num_kvs - 1]->key);
      value.parse(inputs[0].kvs[num_kvs - 1]->key);
      writer->emit(0, &key, &value);
      return;
    }
  }
};

// Accumulate elements {... counter=# ... }
class TopList {
public:

  size_t num_elements;              // Maximum number of elements
  au::list<system::Value> values;   // List of values

  TopList() {
    num_elements = 100;             // Default number of elements
  }

  ~TopList() {
    // Remove all newly created instances
    values.clearList();
  }

  void push(Value *value) {
    double new_num = value->get_value_from_map("counter")->get_double();

    // Push a new element
    std::list<system::Value *>::iterator it_values;
    for (it_values = values.begin(); it_values != values.end(); it_values++) {
      double num = (*it_values)->get_value_from_map("counter")->get_double();
      if (new_num > num) {
        values.insert(it_values, copy(value));
        return;
      }
    }

    if (num_elements > values.size())
      values.push_back(copy(value));
  }

  void get(system::Value *output_value) {
    output_value->set_as_vector();
    std::list<system::Value *>::iterator it_values;
    for (it_values = values.begin(); it_values != values.end(); it_values++) {
      output_value->add_value_to_vector()->copyFrom(*it_values);
    }
  }

private:

  system::Value *copy(system::Value *v) {
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
  samson::system::Value key;
  samson::system::Value value;

  samson::system::Value tmp_value;

  samson::system::Value output_value;

  int time_span;
  double factor;           // Forgetting factor

  size_t current_time;

public:

  void init(std::string command) {
    au::CommandLine cmdLine;

    cmdLine.set_flag_int("time", 360);
    cmdLine.parse(command);

    time_span = cmdLine.get_flag_int("time");

    if (time_span == 0)
      factor = 1;
    else
      factor = ((double)time_span - 1.0 ) / (double)time_span;

    current_time = time(NULL);
  }

  void update_counter(system::Value *value) {
    // Forgetting factor
    size_t time_diff = current_time - value->get_value_from_map("timestamp")->get_double();
    system::Value *counter_value = value->get_value_from_map("counter");
    double previous_counter = counter_value->get_double();
    double counter = previous_counter * pow(factor, time_diff);

    counter_value->set_double(counter);
  }

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Parse common key
    // ------------------------------------

    if (inputs[0].num_kvs > 0)
      key.parse(inputs[0].kvs[0]->key);
    else
      key.parse(inputs[1].kvs[0]->key);

    // Check valid key
    if (key.get_value_from_map("category") == NULL)
      return;



    if (key.get_value_from_map("concept") == NULL)
      return;




    // Recover state if any
    // ------------------------------------

    double total = 0;
    time_t t = time(NULL);

    if (inputs[1].num_kvs > 0) {
      value.parse(inputs[1].kvs[0]->value);
      update_counter(&value);

      // Counter to accumulate inputs
      total = value.get_value_from_map("counter")->get_double();
    } else {
      value.set_as_map();
      // Copy value and category from key
      value.add_value_to_map("category")->copyFrom(key.get_value_from_map("category"));
      value.add_value_to_map("concept")->copyFrom(key.get_value_from_map("concept"));

      // Add time-stamp and counter
      value.add_value_to_map("timestamp")->set_double(t);
      value.add_value_to_map("counter")->set_double(0);
      total = 0;
    }


    // Compute the new total
    for (size_t i = 0; i < inputs[0].num_kvs; i++) {
      tmp_value.parse(inputs[0].kvs[i]->value);
      if (tmp_value.isNumber()) {
        double tmp =  tmp_value.get_double();
        total += tmp;
      }
    }

    // Emit to update the state
    value.add_value_to_map("counter")->set_double(total);
    writer->emit(1, &key, &value);

    // Emit to accumulate by category
    key.set_string(value.get_value_from_map("category")->get_string());
    writer->emit(0, &key, &value);

    return;
  }
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
  samson::system::Value key;
  samson::system::Value value;

  samson::system::Value output_value;

  int time_span;

  double factor;           // Forgetting factor

  TopList top_list;        // List to keep top elements

  size_t current_time;

public:

  void init(std::string command) {
    au::CommandLine cmdLine;

    cmdLine.set_flag_uint64("time_span", 360);
    cmdLine.parse(command);

    time_span = cmdLine.get_flag_int("time");

    if (time_span == 0)
      factor = 1;
    else
      factor = ((double)time_span - 1 ) / (double)time_span;

    current_time = time(NULL);
  }

  void update_counter(system::Value *value) {
    // Forgetting factor
    size_t time_diff = current_time - value->get_value_from_map("timestamp")->get_double();
    system::Value *counter_value = value->get_value_from_map("counter");

    counter_value->set_double(counter_value->get_double() * pow(factor, time_diff));
  }

  void run(samson::KVSetStruct *inputs, samson::KVWriter *writer) {
    // Parse common key
    if (inputs[0].num_kvs > 0)
      key.parse(inputs[0].kvs[0]->key);
    else
      key.parse(inputs[1].kvs[0]->key);

    // Recover elements in the state
    if (inputs[1].num_kvs > 0) {
      value.parse(inputs[1].kvs[0]->value);

      size_t num_elements = value.get_vector_size();
      for (size_t i = 0; i < num_elements; i++) {
        system::Value *v = value.get_value_from_vector(i);
        update_counter(v);
        top_list.push(v);
      }
    }

    // Process all inputs ( if any )
    for (size_t i = 0; i < inputs[0].num_kvs; i++) {
      value.parse(inputs[0].kvs[i]->value);
      top_list.push(&value);
    }

    // Emit to update the state
    top_list.get(&value);

    // Emit state and output
    writer->emit(0, &key, &value);
    writer->emit(1, &key, &value);

    return;
  }
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
  // Map with all available commands
  std::map<std::string, factory_ValueReduce> factories;

public:

  ValueReduceManager(std::string type) {
    if (type == "reduce") {
      add<ValueReduce_all>("all");
      add<ValueReduce_unique>("unique");
      add<ValueReduce_unique_counter>("unique_counter");

      add<ValueReduce_top>("top");                               // Expect key - value
      add<ValueReduce_top_concept>("top_concept");               // Expect key - [ value counter ]

      add<ValueReduce_sum>("sum");
      add<ValueReduce_average>("average");
    }

    if (type == "update") {
      add<ValueReduce_update_last>("last");                      // Keep the last value
      add<ValueReduce_update_sum>("sum");                        // Keep the total sum of seen values

      add<ValueReduce_accumulate>("accumulate");
      // Received     { value:"XX" category:"XX" } , counter
      // State        { value:"XX" category:"XX" } , { value:"XX" category:"XX" counter:# timestamp:# }
      // Emit         category                     , { value:"XX" category:"XX" counter:# timestamp:# }

      add<ValueReduce_accumulate_top>("accumulate_top");
      // Received     category                     , { value:"XX" category:"XX" counter:# timestamp:# }
      // State        category                     , [ { value:"XX" counter:# timestamp:# } ... ]
      // Emit         category                     , [ { value:"XX" counter:# timestamp:# } ... ]
    }
  }

  template<class C>
  void add(std::string name) {
    factories.insert(std::pair<std::string, factory_ValueReduce>(name, factory_ValueReduce_impl<C>));
  }

  ValueReduce *getInstance(std::string command) {
    // Discover the main command
    au::CommandLine cmdLine;

    cmdLine.parse(command);

    if (cmdLine.get_num_arguments() == 0)
      return NULL;




    // Main command
    std::string name = cmdLine.get_argument(0);

    std::map<std::string, factory_ValueReduce>::iterator it_factories;
    it_factories = factories.find(name);

    if (it_factories == factories.end()) {
      return NULL;
    } else {
      ValueReduce *value_reduce =  it_factories->second();
      value_reduce->init(command);
      return value_reduce;
    }
  }

  std::string getListOfCommands() {
    std::ostringstream output;

    std::map<std::string, factory_ValueReduce>::iterator it_factories;
    for (it_factories = factories.begin(); it_factories != factories.end(); it_factories++) {
      output << it_factories->first << " ";
    }
    return output.str();
  }
};
}
}

#endif  // ifndef _H_SAMSON_system_reduce_VALUE_REDUCE
