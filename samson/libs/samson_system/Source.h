
#ifndef _H_SAMSON_SYSTEM_SOURCE
#define _H_SAMSON_SYSTEM_SOURCE

#include "au/StringComponents.h"
#include "au/Tokenizer.h"
#include "au/containers/vector.h"
#include "au/string.h"

#include "samson_system/KeyValue.h"
#include "samson_system/Value.h"
#include "samson_system/ValueContainer.h"
#include <samson/module/samson.h>


namespace samson {
namespace system {
// ---------------------------------------------------
// Source
// ---------------------------------------------------


class Source {
public:

  virtual samson::system::Value *get(KeyValue kv) = 0;
  virtual std::string str() = 0;
  virtual ~Source() {
  }
};

// ---------------------------------------------------
// SourceVoid
// ---------------------------------------------------

class SourceVoid : public Source {
  samson::system::ValueContainer value_container;

public:

  SourceVoid() {
    value_container.value->set_as_void();
  }

  virtual samson::system::Value *get(KeyValue kv) {
    return value_container.value;
  }

  virtual std::string str() {
    return "<void>";
  }
};

// ---------------------------------------------------
// SourceCompare
// ---------------------------------------------------

class SourceCompareSelector : public Source {
  Source *condition;
  Source *value1;
  Source *value2;

public:

  SourceCompareSelector(Source *_condition, Source *_value1, Source *_value2) {
    condition = _condition;
    value1 = _value1;
    value2 = _value2;
  }

  samson::system::Value *get(KeyValue kv) {
    Value *value_condition = condition->get(kv);

    if (!value_condition)
      return NULL;




    if (value_condition->get_double() != 0)
      return value1->get(kv);
    else
      return value2->get(kv);
  }

  std::string str() {
    return au::str("%s?%s:%s", condition->str().c_str(), value1->str().c_str(), value2->str().c_str());
  }
};


// ---------------------------------------------------
// SourceCompare
// ---------------------------------------------------

class SourceCompare : public Source {
public:

  typedef enum {
    equal,                                  // ==
    greater_than,                           // >
    less_than,                              // <
    greater_or_equal_than,                  // >=
    less_or_equal_than,                     // <=
    different_than,                         // !=
    unknown
  } Comparisson;

private:

  Source *left;
  Source *rigth;
  Comparisson comparisson;

  ValueContainer value_container;

public:

  static Comparisson comparition_from_string(std::string s) {
    Comparisson c = unknown;

    if (s == "==")
      c = equal;
    else if (s == "<")
      c = less_than;
    else if (s == ">")
      c = greater_than;
    else if (s == ">=")
      c = greater_or_equal_than;
    else if (s == "<=")
      c = less_or_equal_than;
    else if (s == "!=")
      c = different_than;

    return c;
  }

  static const char *str_Comparisson(Comparisson c) {
    switch (c) {
      case equal:                  return "==";

      case greater_than:           return ">";

      case less_than:              return "<";

      case greater_or_equal_than:  return ">=";

      case less_or_equal_than:     return "<=";

      case different_than:         return "!=";

      case unknown:                return "?";
    }

    LM_X(1, ("Internal error"));
    return "Unknown";
  }

  SourceCompare(Source *_left, Source *_rigth, Comparisson _comparison) {
    rigth = _rigth;
    left = _left;

    comparisson = _comparison;
  }

  bool eval(KeyValue kv) {
    system::Value *v1 = left->get(kv);
    system::Value *v2 = rigth->get(kv);

    // If not possible to get one of them
    if (!v1 || !v2)
      return false;




    switch (comparisson) {
      case equal:                 return ( *v1 == *v2 );

      case greater_than:          return ( *v1 >  *v2 );

      case less_than:             return ( *v1 <  *v2 );

      case greater_or_equal_than: return ( *v1 >= *v2 );

      case less_or_equal_than:    return ( *v1 <= *v2 );

      case different_than:        return ( *v1 != *v2 );

      case unknown: return false;
    }

    // Never here
    return false;
  }

  samson::system::Value *get(KeyValue kv) {
    // Eval an assign 1 for true, 0 for false
    if (eval(kv))
      value_container.value->set_double(1);
    else
      value_container.value->set_double(0);

    return value_container.value;
  }

  std::string str() {
    return au::str("%s %s %s", left->str().c_str(), str_Comparisson(comparisson), rigth->str().c_str());
  }
};

// ---------------------------------------------------
// SourceCompare
// ---------------------------------------------------

class SourceOperation : public Source {
public:

  typedef enum {
    sum,                                 // +
    minus,                               // -
    multiply,                            // *
    divide,                              // /
    unknown
  } Operation;

private:

  Source *left;
  Source *rigth;
  Operation operation;

  ValueContainer value_container;

public:

  static Operation operation_from_string(std::string s) {
    Operation c = unknown;

    if (s == "+")
      c = sum;
    else if (s == "-")
      c = minus;
    else if (s == "*")
      c = multiply;
    else if (s == "/")
      c = divide;

    return c;
  }

  static const char *str_Operation(Operation c) {
    switch (c) {
      case sum:             return "+";

      case minus:           return "-";

      case multiply:        return "*";

      case divide:          return "/";

      case unknown:         return "<Unknown>";
    }

    LM_X(1, ("Internal error"));
    return "Unknown";
  }

  SourceOperation(Source *_left, Source *_rigth, Operation _operation) {
    rigth = _rigth;
    left = _left;

    operation = _operation;
  }

  samson::system::Value *get(KeyValue kv) {
    system::Value *v1 = left->get(kv);
    system::Value *v2 = rigth->get(kv);

    // If not possible to get one of them
    if (!v1 || !v2)
      return NULL;




    switch (operation) {
      case sum:       *value_container.value = *v1 + *v2;
        break;
      case minus:     *value_container.value = *v1 - *v2;
        break;
      case multiply:  *value_container.value = *v1 * *v2;
        break;
      case divide:    *value_container.value = *v1 / *v2;
        break;
      case unknown: return NULL;

        break;
    }

    return value_container.value;
  }

  std::string str() {
    return au::str("%s %s %s", left->str().c_str(), str_Operation(operation), rigth->str().c_str());
  }
};

// ---------------------------------------------------
// SourceStringConstant
// ---------------------------------------------------

class SourceStringConstant : public Source {
  ValueContainer value_container;

public:

  SourceStringConstant(std::string _value) {
    value_container.value->set_string(_value);
  }

  samson::system::Value *get(KeyValue kv) {
    return value_container.value;
  }

  std::string str() {
    return au::str("\"%s\"", value_container.value->get_string().c_str());
  }
};

class SourceNumberConstant : public Source {
  ValueContainer value_container;

public:

  SourceNumberConstant(double _value) {
    value_container.value->set_double(_value);
  }

  samson::system::Value *get(KeyValue kv) {
    return value_container.value;
  }

  std::string str() {
    return au::str("#%s", value_container.value->get_string().c_str());
  }
};


class SourceKey : public Source {
public:

  samson::system::Value *get(KeyValue kv) {
    return kv.key;
  }

  virtual std::string str() {
    return "key";
  }
};

class SourceValue : public Source {
public:

  samson::system::Value *get(KeyValue kv) {
    return kv.value;
  }

  virtual std::string str() {
    return "value";
  }
};


class SourceVectorComponent : public Source {
  Source *base;
  Source *index;

public:

  SourceVectorComponent(Source *_base, Source *_index) {
    base = _base;
    index = _index;
  }

  samson::system::Value *get(KeyValue kv) {
    Value *base_value = base->get(kv);
    Value *index_value = index->get(kv);

    if (!index_value || !base_value)
      return NULL;




    return base_value->get_value_from_vector(index_value->get_double());
  }

  virtual std::string str() {
    return au::str("%s[%s]", base->str().c_str(), index->str().c_str());
  }
};

class SourceMapComponent : public Source {
  Source *base;
  Source *index;

public:

  SourceMapComponent(Source *_base,  Source *_index) {
    base = _base;
    index = _index;
  }

  samson::system::Value *get(KeyValue kv) {
    Value *base_value = base->get(kv);
    Value *index_value = index->get(kv);

    if (!index_value || !base_value)
      return NULL;




    std::string key = index_value->get_string();
    return base_value->get_value_from_map(key);
  }

  virtual std::string str() {
    return au::str("%s:[%s]", base->str().c_str(), index->str().c_str());
  }
};




class SourceVector : public Source {
  au::vector<Source> source_components;
  samson::system::ValueContainer value_container;           // To generate output
public:

  SourceVector(au::vector<Source>& _source_components) {
    for (size_t i = 0; i < _source_components.size(); i++) {
      source_components.push_back(_source_components[i]);
    }
  }

  virtual samson::system::Value *get(KeyValue kv) {
    value_container.value->set_as_vector();
    for (size_t i = 0; i < source_components.size(); i++) {
      samson::system::Value *tmp_value = source_components[i]->get(kv);
      if (!tmp_value)
        value_container.value->add_value_to_vector()->set_as_void();
      else
        value_container.value->add_value_to_vector()->copyFrom(tmp_value);
    }
    return value_container.value;
  }

  virtual std::string str() {
    std::ostringstream output;

    output << "[ ";
    for (size_t i = 0; i < source_components.size(); i++) {
      output << source_components[i]->str() << " ";
    }
    output << "]";
    return output.str();
  }
};

class SourceMap : public Source {
  au::vector<Source> source_keys;
  au::vector<Source> source_values;

  samson::system::ValueContainer value_container;  // To generate output
public:

  SourceMap(au::vector<Source> _source_keys, au::vector<Source> _source_values) {
    if (_source_keys.size() != _source_values.size())
      return;                                     // Error

    for (size_t i = 0; i < _source_keys.size(); i++) {
      source_keys.push_back(_source_keys[i]);
      source_values.push_back(_source_values[i]);
    }
  }

  virtual samson::system::Value *get(KeyValue kv) {
    value_container.value->set_as_map();
    for (size_t i = 0; i < source_keys.size(); i++) {
      samson::system::Value *tmp_key   = source_keys[i]->get(kv);
      samson::system::Value *tmp_value = source_values[i]->get(kv);

      if (!tmp_key || !tmp_value)
        return NULL;




      // Prepare the value
      value_container.value->add_value_to_map(tmp_key->get_string())->copyFrom(tmp_value);
    }

    return value_container.value;
  }

  virtual std::string str() {
    std::ostringstream output;

    output << "{ ";
    for (size_t i = 0; i < source_keys.size(); i++) {
      output << source_keys[i]->str() << ":";
      output << source_values[i]->str() << " ";
    }
    output << "}";
    return output.str();
  }
};


Source *getSource(au::token::TokenVector *token_vector, au::ErrorManager *error);
}
}

#endif  // ifndef _H_SAMSON_SYSTEM_SOURCE

