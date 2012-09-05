#ifndef _H_SAMSON_SYSTEM_SOURCE_FUNCTION
#define _H_SAMSON_SYSTEM_SOURCE_FUNCTION

#include <stdlib.h> //malloc()

#include <iostream>
#include <regex.h>
#include <sstream>
#include <string>
#include <vector>

#include "au/containers/vector.h"
#include "au/string.h"
#include "au/StringComponents.h"
#include "au/Tokenizer.h"
#include "samson_system/KeyValue.h"
#include "samson_system/Source.h"
#include "samson_system/Value.h"
#include "samson_system/ValueContainer.h"
#include "samson/module/samson.h"

namespace samson {
namespace system {
class SourceFunction : public Source {

  public:
    SourceFunction(std::string name, int min_num_arguments = 1, int max_num_arguments = 1000) :
      name_(name), min_num_arguments_(min_num_arguments), max_num_arguments_(max_num_arguments) {

    }

    virtual ~SourceFunction() {
    }

    // Static function to get an instance of SourceFunction
    static SourceFunction *GetSourceForFunction(std::string function_name, au::vector<Source>& input_sources,
        au::ErrorManager *error);

    int min_num_arguments() const {
      return min_num_arguments_;
    }

    int max_num_arguments() const {
      return max_num_arguments_;
    }

    void setInputSource(au::vector<Source>& input_sources) {
      size_t input_sources_size = input_sources.size();
      for (size_t i = 0; i < input_sources_size; ++i) {
        input_sources_.push_back(input_sources[i]);
      }
    }

    virtual std::string str() {
      std::ostringstream output;

      output << name_ << "( ";
      size_t input_sources_size = input_sources_.size();
      for (size_t i = 0; i < input_sources_size; ++i) {
        output << input_sources_[i]->str() << " ";
      }
      output << ")";
      return output.str();
    }

  protected:
    au::vector<Source> input_sources_;

  private:
    std::string name_;
    int min_num_arguments_;
    int max_num_arguments_;
};

// Functions to transform one value into one value
// Now accepts vector and maps ( all fields are processed )

class OneToOne_SourceFunction : public SourceFunction {

  public:
    OneToOne_SourceFunction(std::string name) :
      SourceFunction(name, 1, 1) {
    }

    virtual ~OneToOne_SourceFunction() {
    }

    // Method to transform simple parameters
    virtual samson::system::Value *IndividualGet(samson::system::Value *input) = 0;

    samson::system::Value *get(KeyValue kv) {
      // If no provided input, nothing can be done
      if (input_sources_.size() == 0) {
        return NULL;
      }

      // Take the first ( only one considered )
      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if (!source_value) {
        return NULL;
      }

      if (source_value->IsVector()) {
        // Create a vector transforming individual component
        value_container_.value->SetAsVector();
        size_t source_value_vector_size = source_value->GetVectorSize();
        for (size_t i = 0; i < source_value_vector_size; ++i) {
          // value component
          samson::system::Value *input_component_value = source_value->GetValueFromVector(i);
          samson::system::Value *output_component_value = IndividualGet(input_component_value);

          if (!output_component_value) {
            return NULL; // If one of the components can not be transformed, not return anything
          }
          value_container_.value->AddValueToVector()->copyFrom(output_component_value);
        }
        return value_container_.value;
      } else {
        if (source_value->IsMap()) {
          // Create a map transforming individual component

          value_container_.value->SetAsMap();
          std::vector<std::string> keys = source_value->GetKeysFromMap();
          size_t keys_size = keys.size();
          for (size_t i = 0; i < keys_size; ++i) {
            // value component
            samson::system::Value *input_component_value = source_value->GetValueFromMap(keys[i]);
            samson::system::Value *output_component_value = IndividualGet(input_component_value);

            if (!output_component_value) {
              return NULL; // If one of the components can not be transformed, not return anything
            }
            value_container_.value->AddValueToMap(keys[i])->copyFrom(output_component_value);
          }
          return value_container_.value;
        } else {
          // Transform individual component
          return IndividualGet(source_value);
        }
      }
    }

  private:
    // output for vector/map based outputs
    samson::system::ValueContainer value_container_;
};

class String_OneToOne_SourceFunction : public OneToOne_SourceFunction {

  public:
    String_OneToOne_SourceFunction(std::string name) :
      OneToOne_SourceFunction(name) {
    }

    virtual ~String_OneToOne_SourceFunction() {
    }

    samson::system::Value *IndividualGet(samson::system::Value *input) {
      if (!input->IsString()) {
        return NULL;
      }
      // Get the line to transform
      line_ = input->GetString();
      // Transform to get an output
      return GetFromString(line_);
    }

    // Unique method to implement for single parameter string operations
    virtual samson::system::Value *GetFromString(std::string& input) = 0;

  private:
    std::string line_;
};

class SourceFunction_getSerialization : public SourceFunction {

  public:
    SourceFunction_getSerialization() :
      SourceFunction("getSerialization", 1, 1) {
      buffer_ = static_cast<char *> (malloc(64 * 1024 * 1024));
    }

    ~SourceFunction_getSerialization() {
      free(buffer_);
    }

    samson::system::Value *get(KeyValue kv) {
      if (input_sources_.size() == 0) {
        return NULL;
      }

      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if (!source_value) {
        return NULL;
      }

      // Serialize in buffer
      source_value->serialize(buffer_);
      Value::SerializationCode code = (Value::SerializationCode) buffer_[0];
      value_container_.value->SetString(Value::StrSerializationCode(code));
      return value_container_.value;
    }

  private:
    char *buffer_;
    samson::system::ValueContainer value_container_;
};

class SourceFunction_getSerializationLength : public SourceFunction {

  public:
    SourceFunction_getSerializationLength() :
      SourceFunction("getSerializationLength", 1, 1) {
      buffer_ = static_cast<char *> (malloc(64 * 1024 * 1024));
    }

    ~SourceFunction_getSerializationLength() {
      free(buffer_);
    }

    samson::system::Value *get(KeyValue kv) {
      if (input_sources_.size() == 0) {
        return NULL;
      }

      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if (!source_value) {
        return NULL;
      }

      // Serialize in buffer
      int l = source_value->serialize(buffer_);
      value_container_.value->SetDouble(l);
      return value_container_.value;
    }

  private:
    char *buffer_;
    samson::system::ValueContainer value_container_;
};

class SourceFunction_getType : public SourceFunction {

  public:
    SourceFunction_getType() :
      SourceFunction("getType", 1, 1) {
    }

    samson::system::Value *get(KeyValue kv) {
      if (input_sources_.size() == 0) {
        return NULL;
      }

      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if (!source_value) {
        return NULL;
      }

      // Get the string describing the type
      value_containter.value->SetString(source_value->StrType());
      return value_containter.value;
    }

  private:
    samson::system::ValueContainer value_containter;
};

class SourceFunctionisAlpha : public SourceFunction {

  public:
    SourceFunctionisAlpha() :
      SourceFunction("isAlpha", 1, 1) {
    }

    samson::system::Value *get(KeyValue kv) {
      if (input_sources_.size() == 0) {
        return NULL;
      }

      samson::system::Value *source_value = input_sources_[0]->get(kv);
      if (!source_value) {
        return NULL;
      }

      if (!source_value->IsString()) {
        return NULL;
      }

      std::string line = source_value->GetString();
      size_t line_length = line.length();
      for (size_t i = 0; i < line_length; ++i) {
        if (!isalpha(line[i])) {
          value_container_.value->SetDouble(0);
          return value_container_.value;
        }
      }

      // Return true
      value_container_.value->SetDouble(1);
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;
};

// -----------------------------------------------------------------
// SourceFunction_str
//
//      Transform into a string
// -----------------------------------------------------------------

class SourceFunction_string : public SourceFunction {

  public:
    SourceFunction_string() :
      SourceFunction("string") { // Accepts N parameters concatenating all of them...
    }

    samson::system::Value *get(KeyValue kv) {
      if (input_sources_.size() == 0) {
        return NULL;
      }

      std::string str;
      size_t input_sources_size = input_sources_.size();
      for (size_t i = 0; i < input_sources_size; ++i) {
        samson::system::Value *source_value = input_sources_[i]->get(kv);
        if (!source_value) {
          return NULL;
        }
        str.append(source_value->GetString());
      }
      value_container_.value->SetString(str);
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;
};

// -----------------------------------------------------------------
// SourceFunction_substr
//
//      Substring string, pos , pos
// -----------------------------------------------------------------

class SourceFunction_substr : public SourceFunction {

  public:
    SourceFunction_substr() :
      SourceFunction("substr", 2, 3) {
    }

    samson::system::Value *get(KeyValue kv) {
      if (input_sources_.size() == 0) {
        return NULL;
      }

      // Use only the first component
      samson::system::Value *source_value = input_sources_[0]->get(kv);
      if (!source_value) {
        return NULL;
      }

      samson::system::Value *pos_value = input_sources_[1]->get(kv);
      if (!pos_value) {
        return NULL;
      }

      samson::system::Value *length_value = NULL;
      if (input_sources_.size() > 2) {
        length_value = input_sources_[2]->get(kv);
        if (!length_value) {
          return NULL;
        }
      }

      if (!source_value->IsString()) {
        return NULL;
      }

      if (!pos_value->IsNumber()) {
        return NULL;
      }

      if (length_value) {
        if (!length_value->IsNumber()) {
          return NULL;
        }
      }

      std::string input = source_value->GetString();
      size_t pos = pos_value->GetDouble();

      if (length_value == NULL) {
        std::string output = input.substr(pos);
        value_container_.value->SetString(output);
        return value_container_.value;
      } else {
        size_t length = length_value->GetDouble();
        std::string output = input.substr(pos, length);
        value_container_.value->SetString(output);
        return value_container_.value;
      }
    }

  private:
    samson::system::ValueContainer value_container_;
};

class SourceFunction_find : public SourceFunction {

  public:

    SourceFunction_find() :
      SourceFunction("find", 2, 2) { // find( string , "sub_string" )
    }

    samson::system::Value *get(KeyValue kv) {
      // Use only the first component
      samson::system::Value *main_value = input_sources_[0]->get(kv);
      samson::system::Value *sub_value = input_sources_[1]->get(kv);

      if (!main_value || !sub_value) {
        return NULL;
      }

      std::string main = main_value->GetString();
      std::string sub = sub_value->GetString();

      size_t pos = main.find(sub);

      if (pos == std::string::npos) {
        value_container_.value->SetDouble(-1);
        return value_container_.value;
      } else {
        value_container_.value->SetDouble(pos);
        return value_container_.value;
      }
    }

  private:
    samson::system::ValueContainer value_container_;
};

// -----------------------------------------------------------------
// SourceFunction_number
//
//      Transform into a number
// -----------------------------------------------------------------

class SourceFunction_number : public SourceFunction {

  public:

    SourceFunction_number() :
      SourceFunction("number", 1, 1) {
    }

    samson::system::Value *get(KeyValue kv) {
      if (input_sources_.size() == 0) {
        return NULL;
      }

      // Use only the first component
      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if (!source_value) {
        return NULL;
      }

      value_container_.value->SetDouble(source_value->GetDouble());
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;
};

// -----------------------------------------------------------------
// SourceFunction_json
//
//      Get a JSON string from an value
// -----------------------------------------------------------------

class SourceFunction_json : public SourceFunction {

  public:
    SourceFunction_json() :
      SourceFunction("json", 1, 1) {
    }

    samson::system::Value *get(KeyValue kv) {
      // Use only the first component
      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if (!source_value) {
        return NULL;
      }

      value_container_.value->SetString(source_value->strJSON());
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;

};

// -----------------------------------------------------------------
// SourceFunction_json
//
//      Get a JSON string from an value
// -----------------------------------------------------------------

class SourceFunction_str : public SourceFunction {

  public:

    SourceFunction_str() :
      SourceFunction("str", 1, 1) {
    }

    samson::system::Value *get(KeyValue kv) {
      // Use only the first component
      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if (!source_value) {
        return NULL;
      }

      value_container_.value->SetString(source_value->str());
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;
};

// -----------------------------------------------------------------
// SourceFunction_time
//
//      Get a time(NULL) number
// -----------------------------------------------------------------

class SourceFunction_time : public SourceFunction {

  public:

    SourceFunction_time() :
      SourceFunction("time", 0, 0) {
    }

    samson::system::Value *get(KeyValue kv) {
      value_container_.value->SetDouble(time(NULL));
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;
};

// -----------------------------------------------------------------
// SourceFunction_strlen
// -----------------------------------------------------------------

class SourceFunction_strlen : public String_OneToOne_SourceFunction {

  public:

    SourceFunction_strlen() :
      String_OneToOne_SourceFunction("strlen") {
    }

    samson::system::Value *GetFromString(std::string& input) {
      value_container_.value->SetDouble(input.length());
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;

};

// -----------------------------------------------------------------
// SourceFunction_to_lower
// -----------------------------------------------------------------

class SourceFunction_to_lower : public SourceFunction {

  public:

    SourceFunction_to_lower() :
      SourceFunction("to_lower", 1, 1) , line_(NULL), max_line_size_(0) {
    }

    ~SourceFunction_to_lower() {
      if (line_) {
        free(line_);
      }
    }

    samson::system::Value *get(KeyValue kv) {
      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if ((!source_value) || !source_value->IsString()) {
        return NULL;
      }

      // Recover the input string
      std::string _input_line = source_value->GetString();

      const char *input_line = _input_line.c_str();
      int input_line_size = _input_line.length();

      if (max_line_size_ < (input_line_size + 1)) {
        if (line_) {
          free(line_);
        }

        // Init value
        if (max_line_size_ == 0) {
          max_line_size_ = 1;
        }

        while (max_line_size_ < (input_line_size + 1)) {
          max_line_size_ *= 2;
        }

        line_ = static_cast<char *>(malloc(max_line_size_));
        if (line_ == NULL) {
          LM_E(("Error in malloc for line of %lu bytes", max_line_size_));
          abort();
        }
      }

      strcpy(line_, input_line);

      value_container_.value->SetString(line_);
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;
    char *line_;
    int max_line_size_;
};

class SourceFunction_match : public SourceFunction {

  public:
    SourceFunction_match() :
      SourceFunction("match", 2, 2), compiled_(false), error_(false) {

    }

    ~SourceFunction_match() {
      if (compiled_) {
        regfree(&preg_);
      }
    }

    samson::system::Value *get(KeyValue kv) {
      if (!error_ && !compiled_) {
        samson::system::Value *source_regular_expression = input_sources_[1]->get(kv);
        if (!source_regular_expression) {
          error_ = true;
          return NULL;
        }

        if (!source_regular_expression->IsString()) {
          error_ = true;
          return NULL;
        }

        // Compile regular expression
        regcomp(&preg_, source_regular_expression->GetString().c_str(), 0);
        compiled_ = true;
      }

      if (error_) {
        return NULL;
      }

      // Get the first value
      samson::system::Value *source_value = input_sources_[0]->get(kv);

      // Only accept string values
      if ((!source_value) || !source_value->IsString()) {
        return NULL;
      }

      // Exec regular expression
      int c = regexec(&preg_, source_value->GetString().c_str(), 0, NULL, 0);

      if (c == 0) {
        value_container_.value->SetDouble(1);
      } else {
        value_container_.value->SetDouble(0);
      }

      return value_container_.value;
    }

  private:
    // Value used to emit output
    samson::system::ValueContainer value_container_;
    // Struct to store regular expression
    regex_t preg_;
    // Flag to indicate if the regular expression is compiled
    bool compiled_;
    // Flag to determine that has been an error
    bool error_;
};

// -----------------------------------------------------------------
// SourceFunction_to_upper
// -----------------------------------------------------------------

class SourceFunction_to_upper : public SourceFunction {

  public:

    SourceFunction_to_upper() :
      SourceFunction("to_upper", 1, 1), line_(NULL), max_line_size_(0) {
    }

    ~SourceFunction_to_upper() {
      if (line_) {
        free(line_);
      }
    }

    samson::system::Value *get(KeyValue kv) {
      samson::system::Value *source_value = input_sources_[0]->get(kv);

      if ((!source_value) || !source_value->IsString()) {
        return NULL;
      }

      // Recover the input string
      std::string _input_line = source_value->GetString();
      const char *input_line = _input_line.c_str();
      int input_line_size = _input_line.length();

      if (max_line_size_ < (input_line_size + 1)) {
        if (line_) {
          free(line_);
        }

        // Init value
        if (max_line_size_ == 0) {
          max_line_size_ = 1;
        }

        while (max_line_size_ < (input_line_size + 1)) {
          max_line_size_ *= 2;
        }

        line_ = static_cast<char *>(malloc(max_line_size_));
        if (line_ == NULL) {
          LM_E(("Error in malloc for line of %lu bytes", max_line_size_));
          abort();
        }
      }

      strcpy(line_, input_line);

      value_container_.value->SetString(line_);
      return value_container_.value;
    }

  private:
    samson::system::ValueContainer value_container_;
    char *line_;
    int max_line_size_;
};

// -----------------------------------------------------------------
// SourceFunctionManager
//
// Manager for all source functions
// -----------------------------------------------------------------

typedef SourceFunction *(*factory_SourceFunction)();

template<class C>
SourceFunction *FactorySourceFunctionImpl() {
  return new C();
}

class SourceFunctionManager {

  public:

    SourceFunctionManager() {
      // String functions
      Add<SourceFunction_strlen> ("strlen");
      Add<SourceFunction_substr> ("substr");
      Add<SourceFunction_find> ("find");
      Add<SourceFunction_to_lower> ("to_lower");
      Add<SourceFunction_to_upper> ("to_upper");
      Add<SourceFunction_match> ("match");

      // Output strings
      Add<SourceFunction_json> ("json");
      Add<SourceFunction_str> ("str");

      // Transform into something
      Add<SourceFunction_string> ("string");
      Add<SourceFunction_number> ("number");

      // Check properties
      Add<SourceFunctionisAlpha> ("isAlpha");

      // Get information about type and serialization of the value
      Add<SourceFunction_getType> ("getType");
      Add<SourceFunction_getSerialization> ("getSerialization");
      Add<SourceFunction_getSerializationLength> ("getSerializationLength");
    }

    template<class C>
    void Add(std::string name) {
      factories_.insert(std::pair<std::string, factory_SourceFunction>(name, FactorySourceFunctionImpl<C> ));
    }

    SourceFunction *getInstance(std::string name, au::vector<Source>& input_sources, au::ErrorManager *error) {
      // Find in map
      std::map<std::string, factory_SourceFunction>::iterator it_factories;
      it_factories = factories_.find(name);

      if (it_factories == factories_.end()) {
        error->set(au::str("Function %s not found", name.c_str()));
        return NULL;
      }

      SourceFunction *source_function = it_factories->second();

      int num_arguments = (int) input_sources.size();
      int min_num_arguments = source_function->min_num_arguments();
      int max_num_arguments = source_function->max_num_arguments();

      if (num_arguments < min_num_arguments) {
        delete source_function;
        error->set(au::str("Function %s requires at least %d parameters. Only %d provded", name.c_str(),
                           min_num_arguments, num_arguments));
        return NULL;
      }

      if (num_arguments > max_num_arguments) {
        delete source_function;
        error->set(au::str("Function %s cannot accepts more than %d parameters. Provided %d arguments", name.c_str(),
                           min_num_arguments, num_arguments));
        return NULL;
      }

      source_function->setInputSource(input_sources);
      return source_function;
    }

  private:
    std::map<std::string, factory_SourceFunction> factories_;
};
}
}

#endif  // ifndef _H_SAMSON_SYSTEM_SOURCE_FUNCTION
