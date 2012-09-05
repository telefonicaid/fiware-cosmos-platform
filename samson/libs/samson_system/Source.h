#ifndef _H_SAMSON_SYSTEM_SOURCE
#define _H_SAMSON_SYSTEM_SOURCE

#include <iostream>
#include <sstream>
#include <string>

#include "au/StringComponents.h"
#include "au/Tokenizer.h"
#include "au/containers/vector.h"
#include "au/string.h"

#include "samson_system/KeyValue.h"
#include "samson_system/Value.h"
#include "samson_system/ValueContainer.h"
#include "samson/module/samson.h"

namespace samson {
namespace system {
// ---------------------------------------------------
// Source
// ---------------------------------------------------


class Source {
  public:

    virtual ~Source() {
    }

    virtual samson::system::Value *get(KeyValue kv) = 0;
    virtual std::string str() = 0;

};

// ---------------------------------------------------
// SourceVoid
// ---------------------------------------------------

class SourceVoid : public Source {

  public:

    SourceVoid() {
      value_container_.value->SetAsVoid();
    }

    virtual samson::system::Value *get(KeyValue kv) {
      return value_container_.value;
    }

    virtual std::string str() {
      return "<void>";
    }
  private:
    samson::system::ValueContainer value_container_;
};

// ---------------------------------------------------
// SourceCompare
// ---------------------------------------------------

class SourceCompareSelector : public Source {

  public:

    SourceCompareSelector(Source *condition, Source *value1, Source *value2)
      : condition_(condition)
      , value1_(value1)
      , value2_(value2) {
    }

    samson::system::Value *get(KeyValue kv) {
      Value *value_condition = condition_->get(kv);

      if (!value_condition) {
        return NULL;
      }

      if (value_condition->GetDouble() != 0) {
        return value1_->get(kv);
      } else {
        return value2_->get(kv);
      }
    }

    std::string str() {
      return au::str("%s?%s:%s", condition_->str().c_str(), value1_->str().c_str(), value2_->str().c_str());
    }

  private:
    Source *condition_;
    Source *value1_;
    Source *value2_;
};

// ---------------------------------------------------
// SourceCompare
// ---------------------------------------------------

class SourceCompare : public Source {
  public:

    typedef enum {
      equal, // ==
      greater_than, // >
      less_than, // <
      greater_or_equal_than, // >=
      less_or_equal_than, // <=
      different_than, // !=
      unknown
    } Comparisson;

  public:

    static Comparisson comparition_from_string(std::string s) {
      Comparisson c = unknown;

      if (s == "==") {
        c = equal;
      } else {
        if (s == "<") {
          c = less_than;
        } else {
          if (s == ">") {
            c = greater_than;
          } else {
            if (s == ">=") {
              c = greater_or_equal_than;
            } else {
              if (s == "<=") {
                c = less_or_equal_than;
              } else {
                if (s == "!=") {
                  c = different_than;
                }
              }
            }
          }
        }
      }

      return c;
    }

    static const char *str_Comparisson(Comparisson c) {
      switch (c) {
        case equal:
          return "==";

        case greater_than:
          return ">";

        case less_than:
          return "<";

        case greater_or_equal_than:
          return ">=";

        case less_or_equal_than:
          return "<=";

        case different_than:
          return "!=";

        case unknown:
          return "?";
      }

      LM_X(1, ("Internal error"));
      return "Unknown";
    }

    SourceCompare(Source *left, Source *right, Comparisson comparison)
        : left_(left)
        , right_(right)
        , comparisson_(comparison) {
      value_container_.value->SetAsVoid();
    }

    bool eval(KeyValue kv) {
      system::Value *v1 = left_->get(kv);
      system::Value *v2 = right_->get(kv);

      // If not possible to get one of them
      if (!v1 || !v2) {
        return false;
      }

      switch (comparisson_) {
        case equal:
          return (*v1 == *v2);
        case greater_than:
          return (*v1 > *v2);
        case less_than:
          return (*v1 < *v2);
        case greater_or_equal_than:
          return (*v1 >= *v2);
        case less_or_equal_than:
          return (*v1 <= *v2);
        case different_than:
          return (*v1 != *v2);
        case unknown:
          return false;
      }

      // Never here
      return false;
    }

    samson::system::Value *get(KeyValue kv) {
      // Eval an assign 1 for true, 0 for false
      if (eval(kv)) {
        value_container_.value->SetDouble(1);
      } else {
        value_container_.value->SetDouble(0);
      }

      return value_container_.value;
    }

    std::string str() {
      return au::str("%s %s %s", left_->str().c_str(), str_Comparisson(comparisson_), right_->str().c_str());
    }

  private:

    Source *left_;
    Source *right_;
    Comparisson comparisson_;

    ValueContainer value_container_;

};

// ---------------------------------------------------
// SourceCompare
// ---------------------------------------------------

class SourceOperation : public Source {
  public:

    typedef enum {
      sum, // +
      minus, // -
      multiply, // *
      divide, // /
      unknown
    } Operation;

  public:

    static Operation operation_from_string(std::string s) {
      Operation c = unknown;

      if (s == "+") {
        c = sum;
      } else {
        if (s == "-") {
          c = minus;
        } else {
          if (s == "*") {
            c = multiply;
          } else {
            if (s == "/") {
              c = divide;
            }
          }
        }
      }
      return c;
    }

    static const char *str_Operation(Operation c) {
      switch (c) {
        case sum:
          return "+";

        case minus:
          return "-";

        case multiply:
          return "*";

        case divide:
          return "/";

        case unknown:
          return "<Unknown>";
      }

      LM_X(1, ("Internal error"));
      return "Unknown";
    }

    SourceOperation(Source *left, Source *right, Operation operation)
      : left_(left)
      , right_(right)
      , operation_(operation) {
      value_container_.value->SetAsVoid();
    }

    samson::system::Value *get(KeyValue kv) {
      system::Value *v1 = left_->get(kv);
      system::Value *v2 = right_->get(kv);

      // If not possible to get one of them
      if (!v1 || !v2) {
        return NULL;
      }

      switch (operation_) {
        case sum:
          *value_container_.value = *v1 + *v2;
          break;
        case minus:
          *value_container_.value = *v1 - *v2;
          break;
        case multiply:
          *value_container_.value = *v1 * *v2;
          break;
        case divide:
          *value_container_.value = *v1 / *v2;
          break;
        case unknown:
          return NULL;

          break;
      }

      return value_container_.value;
    }

    std::string str() {
      return au::str("%s %s %s", left_->str().c_str(), str_Operation(operation_), right_->str().c_str());
    }

  private:

    Source *left_;
    Source *right_;
    Operation operation_;

    ValueContainer value_container_;

};

// ---------------------------------------------------
// SourceStringConstant
// ---------------------------------------------------

class SourceStringConstant : public Source {

  public:

    explicit SourceStringConstant(std::string _value) {
      value_container_.value->SetString(_value);
    }

    samson::system::Value *get(KeyValue kv) {
      return value_container_.value;
    }

    std::string str() {
      return au::str("\"%s\"", value_container_.value->GetString().c_str());
    }

  private:
    ValueContainer value_container_;
};

class SourceNumberConstant : public Source {

  public:

    explicit SourceNumberConstant(double _value) {
      value_container_.value->SetDouble(_value);
    }

    samson::system::Value *get(KeyValue kv) {
      return value_container_.value;
    }

    std::string str() {
      return au::str("#%s", value_container_.value->GetString().c_str());
    }

  private:
    ValueContainer value_container_;

};

class SourceKey : public Source {
  public:

    samson::system::Value *get(KeyValue kv) {
      return kv.key();
    }

    virtual std::string str() {
      return "key";
    }
};

class SourceValue : public Source {
  public:

    samson::system::Value *get(KeyValue kv) {
      return kv.value();
    }

    virtual std::string str() {
      return "value";
    }
};

class SourceVectorComponent : public Source {

  public:

    SourceVectorComponent(Source *_base, Source *_index) {
      base_ = _base;
      index_ = _index;
    }

    samson::system::Value *get(KeyValue kv) {
      Value *base_value = base_->get(kv);
      Value *index_value = index_->get(kv);

      if (!index_value || !base_value) {
        return NULL;
      }

      return base_value->GetValueFromVector(index_value->GetDouble());
    }

    virtual std::string str() {
      return au::str("%s[%s]", base_->str().c_str(), index_->str().c_str());
    }

  private:
    Source *base_;
    Source *index_;

};

class SourceMapComponent : public Source {

  public:

    SourceMapComponent(Source *_base, Source *_index) {
      base_ = _base;
      index_ = _index;
    }

    samson::system::Value *get(KeyValue kv) {
      Value *base_value = base_->get(kv);
      Value *index_value = index_->get(kv);

      if (!index_value || !base_value) {
        return NULL;
      }

      std::string key = index_value->GetString();
      return base_value->GetValueFromMap(key);
    }

    virtual std::string str() {
      return au::str("%s:[%s]", base_->str().c_str(), index_->str().c_str());
    }

  private:
    Source *base_;
    Source *index_;
};

class SourceVector : public Source {
  public:

    SourceVector(au::vector<Source>& _source_components) {
      for (size_t i = 0; i < _source_components.size(); i++) {
        source_components_.push_back(_source_components[i]);
      }
    }

    virtual samson::system::Value *get(KeyValue kv) {
      value_container_.value->SetAsVector();
      for (size_t i = 0; i < source_components_.size(); i++) {
        samson::system::Value *tmp_value = source_components_[i]->get(kv);
        if (!tmp_value) {
          value_container_.value->AddValueToVector()->SetAsVoid();
        } else {
          value_container_.value->AddValueToVector()->copyFrom(tmp_value);
        }
      }
      return value_container_.value;
    }

    virtual std::string str() {
      std::ostringstream output;

      output << "[ ";
      for (size_t i = 0; i < source_components_.size(); i++) {
        output << source_components_[i]->str() << " ";
      }
      output << "]";
      return output.str();
    }
  private:
    au::vector<Source> source_components_;
    samson::system::ValueContainer value_container_; // To generate output

};

class SourceMap : public Source {

  public:

    SourceMap(au::vector<Source> _source_keys, au::vector<Source> _source_values) {
      if (_source_keys.size() != _source_values.size()) {
        return; // Error
      }
      for (size_t i = 0; i < _source_keys.size(); i++) {
        source_keys_.push_back(_source_keys[i]);
        source_values_.push_back(_source_values[i]);
      }
    }

    virtual samson::system::Value *get(KeyValue kv) {
      value_container_.value->SetAsMap();
      for (size_t i = 0; i < source_keys_.size(); i++) {
        samson::system::Value *tmp_key = source_keys_[i]->get(kv);
        samson::system::Value *tmp_value = source_values_[i]->get(kv);

        if (!tmp_key || !tmp_value) {
          return NULL;
        }

        // Prepare the value
        value_container_.value->AddValueToMap(tmp_key->GetString())->copyFrom(tmp_value);
      }

      return value_container_.value;
    }

    virtual std::string str() {
      std::ostringstream output;

      output << "{ ";
      for (size_t i = 0; i < source_keys_.size(); i++) {
        output << source_keys_[i]->str() << ":";
        output << source_values_[i]->str() << " ";
      }
      output << "}";
      return output.str();
    }

  private:
    au::vector<Source> source_keys_;
    au::vector<Source> source_values_;

    samson::system::ValueContainer value_container_; // To generate output
};

Source *GetSource(au::token::TokenVector *token_vector, au::ErrorManager *error);

}
}

#endif  // ifndef _H_SAMSON_SYSTEM_SOURCE
