
#ifndef _H_SAMSON_SYSTEM_SOURCE_FUNCTION
#define _H_SAMSON_SYSTEM_SOURCE_FUNCTION

#include "au/StringComponents.h"
#include "au/Tokenizer.h"
#include "au/containers/vector.h"
#include "au/string.h"
#include <regex.h>
// #include "au/charset.h"

#include "samson_system/KeyValue.h"
#include "samson_system/Source.h"
#include "samson_system/Value.h"
#include "samson_system/ValueContainer.h"
#include <samson/module/samson.h>


namespace samson {
namespace system {
class SourceFunction : public Source {
  std::string name;
  int min_num_arguments;
  int max_num_arguments;

protected:

  au::vector<Source> input_sources;

public:

  // Static function to get an instance of SourceFunction
  static SourceFunction *getSourceForFunction(std::string function_name
                                              , au::vector<Source>& _input_sources
                                              , au::ErrorManager *error);


  SourceFunction(std::string _name, int _min_num_arguments = 1, int _max_num_arguments = 1000) {
    name = _name;
    min_num_arguments = _min_num_arguments;
    max_num_arguments = _max_num_arguments;
  }

  int getNumMinArguments() {
    return min_num_arguments;
  }

  int getNumMaxArguments() {
    return max_num_arguments;
  }

  void setInputSource(au::vector<Source>& _input_sources) {
    for (size_t i = 0; i < _input_sources.size(); i++) {
      input_sources.push_back(_input_sources[i]);
    }
  }

  virtual std::string str() {
    std::ostringstream output;

    output << name << "( ";
    for (size_t i = 0; i < input_sources.size(); i++) {
      output << input_sources[i]->str() << " ";
    }
    output << ")";
    return output.str();
  }
};

// Functions to transform one value into one value
// Now accepts vector and maps ( all fields are processed )

class OneToOne_SourceFunction : public SourceFunction {
  // output for vector/map based outputs
  samson::system::ValueContainer value_container;

public:

  OneToOne_SourceFunction(std::string name) : SourceFunction(name, 1, 1) {
  }

  // Method to transform simple parameters
  virtual samson::system::Value *individual_get(samson::system::Value *input) = 0;


  samson::system::Value *get(KeyValue kv) {
    // If no provided input, nothing can be done
    if (input_sources.size() == 0) {
      return NULL;
    }



    // Take the first ( only one considered )
    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (!source_value) {
      return NULL;
    }



    if (source_value->isVector()) {
      // Create a vector transforming individual component
      value_container.value->set_as_vector();
      for (size_t i = 0; i < source_value->get_vector_size(); i++) {
        // value component
        samson::system::Value *input_component_value = source_value->get_value_from_vector(i);
        samson::system::Value *output_component_value = individual_get(input_component_value);

        if (!output_component_value) {
          return NULL;                    // If one of the components can not be transformed, not return anything
        }
        value_container.value->add_value_to_vector()->copyFrom(output_component_value);
      }
      return value_container.value;
    } else if (source_value->isMap()) {
      // Create a map transforming individual component

      value_container.value->set_as_map();
      std::vector<std::string> keys = source_value->get_keys_from_map();
      for (size_t i = 0; i < keys.size(); i++) {
        // value component
        samson::system::Value *input_component_value = source_value->get_value_from_map(keys[i]);
        samson::system::Value *output_component_value = individual_get(input_component_value);

        if (!output_component_value) {
          return NULL;                    // If one of the components can not be transformed, not return anything
        }
        value_container.value->add_value_to_map(keys[i])->copyFrom(output_component_value);
      }
      return value_container.value;

      return NULL;
    } else {
      // Transform individual component
      return individual_get(source_value);
    }
  }
};



class String_OneToOne_SourceFunction : public OneToOne_SourceFunction {
  std::string line;

public:

  String_OneToOne_SourceFunction(std::string name) : OneToOne_SourceFunction(name) {
  }

  samson::system::Value *individual_get(samson::system::Value *input) {
    if (!input->isString()) {
      return NULL;
    }



    // Get the line to transform
    line = input->get_string();

    // Transform to get an output
    return get_from_string(line);
  }

  // Unique method to implement for single parameter string operations
  virtual samson::system::Value *get_from_string(std::string& input) = 0;
};




class SourceFunction_getSerialitzation : public SourceFunction {
  char *buffer;
  samson::system::ValueContainer value_container;

public:

  SourceFunction_getSerialitzation() : SourceFunction("getSerialitzation", 1, 1) {
    buffer = (char *)malloc(64 * 1024 * 1024);
  }

  ~SourceFunction_getSerialitzation() {
    free(buffer);
  }

  samson::system::Value *get(KeyValue kv) {
    if (input_sources.size() == 0) {
      return NULL;
    }



    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (!source_value) {
      return NULL;
    }



    // Serialize in buffer
    source_value->serialize(buffer);

    Value::SerialitzationCode code = (Value::SerialitzationCode)buffer[0];

    value_container.value->set_string(Value::strSerialitzationCode(code));

    return value_container.value;
  }
};

class SourceFunction_getSerialitzationLength : public SourceFunction {
  char *buffer;
  samson::system::ValueContainer value_container;

public:

  SourceFunction_getSerialitzationLength() : SourceFunction("getSerialitzationLength", 1, 1) {
    buffer = (char *)malloc(64 * 1024 * 1024);
  }

  ~SourceFunction_getSerialitzationLength() {
    free(buffer);
  }

  samson::system::Value *get(KeyValue kv) {
    if (input_sources.size() == 0) {
      return NULL;
    }



    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (!source_value) {
      return NULL;
    }



    // Serialize in buffer
    int l = source_value->serialize(buffer);

    value_container.value->set_double(l);

    return value_container.value;
  }
};

class SourceFunction_getType : public SourceFunction {
  samson::system::ValueContainer value_containter;

public:

  SourceFunction_getType() : SourceFunction("getType", 1, 1) {
  }

  samson::system::Value *get(KeyValue kv) {
    if (input_sources.size() == 0) {
      return NULL;
    }



    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (!source_value) {
      return NULL;
    }



    // Get the string describing the type
    value_containter.value->set_string(source_value->strType());

    return value_containter.value;
  }
};

class SourceFunctionisAlpha : public SourceFunction {
  samson::system::ValueContainer value_container;

public:

  SourceFunctionisAlpha() : SourceFunction("isAlpha", 1, 1) {
  }

  samson::system::Value *get(KeyValue kv) {
    if (input_sources.size() == 0) {
      return NULL;
    }



    samson::system::Value *source_value = input_sources[0]->get(kv);
    if (!source_value) {
      return NULL;
    }



    if (!source_value->isString()) {
      return NULL;
    }



    std::string line = source_value->get_string();


    for (size_t i = 0; i < line.length(); i++) {
      if (!isalpha(line[i])) {
        value_container.value->set_double(0);
        return value_container.value;
      }
    }

    // Return true
    value_container.value->set_double(1);
    return value_container.value;
  }
};

// -----------------------------------------------------------------
// SourceFunction_str
//
//      Transform into a string
// -----------------------------------------------------------------

class SourceFunction_string : public SourceFunction {
  samson::system::ValueContainer value_container;

public:

  SourceFunction_string() : SourceFunction("string") {           // Accepts N parameters concatenating all of them...
  }

  samson::system::Value *get(KeyValue kv) {
    if (input_sources.size() == 0) {
      return NULL;
    }



    std::string str;

    for (size_t i = 0; i < input_sources.size(); i++) {
      samson::system::Value *source_value = input_sources[i]->get(kv);
      if (!source_value) {
        return NULL;
      }


      str.append(source_value->get_string());
    }
    value_container.value->set_string(str);
    return value_container.value;
  }
};


// -----------------------------------------------------------------
// SourceFunction_substr
//
//      Substring string, pos , pos
// -----------------------------------------------------------------

class SourceFunction_substr : public SourceFunction {
  samson::system::ValueContainer value_container;

public:

  SourceFunction_substr() : SourceFunction("substr", 2, 3) {
  }

  samson::system::Value *get(KeyValue kv) {
    if (input_sources.size() == 0) {
      return NULL;
    }



    // Use only the first component
    samson::system::Value *source_value = input_sources[0]->get(kv);
    if (!source_value) {
      return NULL;
    }



    samson::system::Value *pos_value    = input_sources[1]->get(kv);
    if (!pos_value) {
      return NULL;
    }



    samson::system::Value *length_value = NULL;
    if (input_sources.size() > 2) {
      length_value = input_sources[2]->get(kv);
      if (!length_value) {
        return NULL;
      }
    }

    if (!source_value->isString()) {
      return NULL;
    }


    if (!pos_value->isNumber()) {
      return NULL;
    }


    if (length_value) {
      if (!length_value->isNumber()) {
        return NULL;
      }
    }




    std::string input  = source_value->get_string();
    size_t pos = pos_value->get_double();

    if (length_value == NULL) {
      std::string output = input.substr(pos);
      value_container.value->set_string(output);
      return value_container.value;
    } else {
      size_t length = length_value->get_double();
      std::string output = input.substr(pos, length);
      value_container.value->set_string(output);
      return value_container.value;
    }
  }
};


class SourceFunction_find : public SourceFunction {
  samson::system::ValueContainer value_container;

public:

  SourceFunction_find() : SourceFunction("find", 2, 2) {             // find( string , "sub_string" )
  }

  samson::system::Value *get(KeyValue kv) {
    // Use only the first component
    samson::system::Value *main_value = input_sources[0]->get(kv);
    samson::system::Value *sub_value = input_sources[1]->get(kv);

    if (!main_value || !sub_value) {
      return NULL;
    }




    std::string main = main_value->get_string();
    std::string sub = sub_value->get_string();

    size_t pos = main.find(sub);

    if (pos == std::string::npos) {
      value_container.value->set_double(-1);
      return value_container.value;
    } else {
      value_container.value->set_double(pos);
      return value_container.value;
    }
  }
};

// -----------------------------------------------------------------
// SourceFunction_number
//
//      Transform into a number
// -----------------------------------------------------------------

class SourceFunction_number : public SourceFunction {
  samson::system::ValueContainer value_container;

public:

  SourceFunction_number() : SourceFunction("number", 1, 1) {
  }

  samson::system::Value *get(KeyValue kv) {
    if (input_sources.size() == 0) {
      return NULL;
    }



    // Use only the first component
    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (!source_value) {
      return NULL;
    }



    value_container.value->set_double(source_value->get_double());
    return value_container.value;
  }
};

// -----------------------------------------------------------------
// SourceFunction_json
//
//      Get a JSON string from an value
// -----------------------------------------------------------------

class SourceFunction_json : public SourceFunction {
  samson::system::ValueContainer value_container;

public:

  SourceFunction_json() : SourceFunction("json", 1, 1) {
  }

  samson::system::Value *get(KeyValue kv) {
    // Use only the first component
    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (!source_value) {
      return NULL;
    }



    value_container.value->set_string(source_value->strJSON());
    return value_container.value;
  }
};

// -----------------------------------------------------------------
// SourceFunction_json
//
//      Get a JSON string from an value
// -----------------------------------------------------------------

class SourceFunction_str : public SourceFunction {
  samson::system::ValueContainer value_container;

public:

  SourceFunction_str() : SourceFunction("str", 1, 1) {
  }

  samson::system::Value *get(KeyValue kv) {
    // Use only the first component
    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (!source_value) {
      return NULL;
    }



    value_container.value->set_string(source_value->str());
    return value_container.value;
  }
};

// -----------------------------------------------------------------
// SourceFunction_time
//
//      Get a time(NULL) number
// -----------------------------------------------------------------

class SourceFunction_time : public SourceFunction {
  samson::system::ValueContainer value_container;

public:

  SourceFunction_time() : SourceFunction("time", 0, 0) {
  }

  samson::system::Value *get(KeyValue kv) {
    value_container.value->set_double(time(NULL));
    return value_container.value;
  }
};

// -----------------------------------------------------------------
// SourceFunction_strlen
// -----------------------------------------------------------------

class SourceFunction_strlen : public String_OneToOne_SourceFunction {
  samson::system::ValueContainer value_container;
public:

  SourceFunction_strlen() : String_OneToOne_SourceFunction("strlen") {
  }

  samson::system::Value *get_from_string(std::string& input) {
    value_container.value->set_double(input.length());
    return value_container.value;
  }
};

// -----------------------------------------------------------------
// SourceFunction_to_lower
// -----------------------------------------------------------------

class SourceFunction_to_lower : public SourceFunction {
  samson::system::ValueContainer value_container;

  char *line;
  int max_line_size;

public:

  SourceFunction_to_lower() : SourceFunction("to_lower", 1, 1) {
    line = NULL;
    max_line_size = 0;
  }

  ~SourceFunction_to_lower() {
    if (line) {
      free(line);
    }
  }

  samson::system::Value *get(KeyValue kv) {
    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (( !source_value ) || !source_value->isString()) {
      return NULL;
    }



    // Recover the input string
    std::string _input_line = source_value->get_string();

    const char *input_line = _input_line.c_str();
    int input_line_size = _input_line.length();

    if (max_line_size < (input_line_size + 1)) {
      if (line) {
        free(line);
      }

      // Init value
      if (max_line_size == 0) {
        max_line_size = 1;
      }

      while (max_line_size < (input_line_size + 1)) {
        max_line_size *= 2;
      }

      line = (char *)malloc(max_line_size);
    }

    for (int i = 0; i < input_line_size; i++) {
      line[i] =  input_line[i];
    }
    //                    line[i] =  au::iso_8859_to_lower( input_line[i] );

    line[input_line_size] = 0;

    value_container.value->set_string(line);
    return value_container.value;
  }
};


class SourceFunction_match : public SourceFunction {
  // Value used to emit output
  samson::system::ValueContainer value_container;

  // Struct to store regular expression
  regex_t preg;

  // Flag to indicate if the regular expressio is compiled
  bool compiled;

  // Flag to determine that has been an error
  bool error;
public:

  SourceFunction_match() : SourceFunction("match", 2, 2) {
    compiled = false;
    error =  false;
  }

  ~SourceFunction_match() {
    if (compiled) {
      regfree(&preg);
    }
  }

  samson::system::Value *get(KeyValue kv) {
    if (!error && !compiled) {
      samson::system::Value *source_regular_expression = input_sources[1]->get(kv);
      if (!source_regular_expression) {
        error = true;
        return NULL;
      }

      if (!source_regular_expression->isString()) {
        error = true;
        return NULL;
      }

      // Compile regular expression
      regcomp(&preg,  source_regular_expression->get_string().c_str(), 0);
      compiled = true;
    }

    if (error) {
      return NULL;
    }



    // Get the first value
    samson::system::Value *source_value = input_sources[0]->get(kv);

    // Only accept string values
    if (( !source_value ) || !source_value->isString()) {
      return NULL;
    }



    // Exec regular expression
    int c = regexec(&preg, source_value->get_string().c_str(), 0, NULL, 0);


    if (c == 0) {
      value_container.value->set_double(1);
    } else {
      value_container.value->set_double(0);
    }

    return value_container.value;
  }
};

// -----------------------------------------------------------------
// SourceFunction_to_upper
// -----------------------------------------------------------------

class SourceFunction_to_upper : public SourceFunction {
  samson::system::ValueContainer value_container;

  char *line;
  int max_line_size;

public:

  SourceFunction_to_upper() : SourceFunction("to_upper", 1, 1) {
    max_line_size = 0;
    line = NULL;
  }

  ~SourceFunction_to_upper() {
    if (line) {
      free(line);
    }
  }

  samson::system::Value *get(KeyValue kv) {
    samson::system::Value *source_value = input_sources[0]->get(kv);

    if (( !source_value ) || !source_value->isString()) {
      return NULL;
    }



    // Recover the input string
    std::string _input_line = source_value->get_string();
    const char *input_line = _input_line.c_str();
    int input_line_size = _input_line.length();

    if (max_line_size < (input_line_size + 1)) {
      if (line) {
        free(line);
      }

      // Init value
      if (max_line_size == 0) {
        max_line_size = 1;
      }

      while (max_line_size < (input_line_size + 1)) {
        max_line_size *= 2;
      }

      line = (char *)malloc(max_line_size);
    }

    for (int i = 0; i < input_line_size; i++) {
      line[i] = input_line[i];
    }
    // line[i] =  au::iso_8859_to_upper( input_line[i] );
    line[input_line_size] = 0;

    value_container.value->set_string(line);
    return value_container.value;
  }
};

// -----------------------------------------------------------------
// SourceFunctionManager
//
// Manager for all source functions
// -----------------------------------------------------------------

typedef SourceFunction *(*factory_SourceFunction)();

template<class C>
SourceFunction *factory_SourceFunction_impl() {
  return new C();
}

class SourceFunctionManager {
  std::map<std::string, factory_SourceFunction> factories;

public:

  SourceFunctionManager() {
    // String functions
    add<SourceFunction_strlen>("strlen");
    add<SourceFunction_substr>("substr");
    add<SourceFunction_find>("find");
    add<SourceFunction_to_lower>("to_lower");
    add<SourceFunction_to_upper>("to_upper");
    add<SourceFunction_match>("match");

    // Output strings
    add<SourceFunction_json>("json");
    add<SourceFunction_str>("str");

    // Transform into something
    add<SourceFunction_string>("string");
    add<SourceFunction_number>("number");

    // Check properties
    add<SourceFunctionisAlpha>("isAlpha");

    // Get information about type and serialitzation of the value
    add<SourceFunction_getType>("getType");
    add<SourceFunction_getSerialitzation>("getSerialitzation");
    add<SourceFunction_getSerialitzationLength>("getSerialitzationLength");
  }

  template<class C>
  void add(std::string name) {
    factories.insert(std::pair<std::string, factory_SourceFunction>(name, factory_SourceFunction_impl<C>));
  }

  SourceFunction *getInstance(std::string name, au::vector<Source>& input_sources, au::ErrorManager *error) {
    // Find in map
    std::map<std::string, factory_SourceFunction>::iterator it_factories;
    it_factories = factories.find(name);

    if (it_factories == factories.end()) {
      error->set(au::str("Function %s not found", name.c_str()));
      return NULL;
    }

    SourceFunction *source_function = it_factories->second();

    int num_arguments = (int)input_sources.size();
    int min_num_arguments = source_function->getNumMinArguments();
    int max_num_arguments = source_function->getNumMaxArguments();

    if (num_arguments < min_num_arguments) {
      delete source_function;
      error->set(au::str("Function %s requires at least %d parameters. Only %d provded"
                         , name.c_str()
                         , min_num_arguments
                         , num_arguments));
      return NULL;
    }

    if (num_arguments > max_num_arguments) {
      delete source_function;
      error->set(au::str("Function %s cannot accepts more than %d parameters. Provided %d arguments"
                         , name.c_str()
                         , min_num_arguments
                         , num_arguments));
      return NULL;
    }

    source_function->setInputSource(input_sources);
    return source_function;
  }
};
}
}

#endif  // ifndef _H_SAMSON_SYSTEM_SOURCE_FUNCTION

