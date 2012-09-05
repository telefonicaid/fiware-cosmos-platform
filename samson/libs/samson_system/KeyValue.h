
#ifndef _H_SAMSON_SYSTEM_KEYVALUE
#define _H_SAMSON_SYSTEM_KEYVALUE

#include "samson_system/Value.h"


namespace samson {
namespace system {
// -----------------------------------------------------------------------
// KeyValue : system.Value system.Value
// -----------------------------------------------------------------------

class KeyValue {
public:


  // Constructor
  KeyValue() : key_(NULL), value_(NULL) {
  }

  KeyValue(Value *key, Value *value) :
    key_(key), value_(value) {
  }

  Value *key() const { return key_; }
  Value *value() const { return value_; }
  void set_key(Value *key) { key_ = key; }
  void set_value(Value *value) { value_ = value; }

private:
Value *key_;
Value *value_;

};
}
}

#endif  // ifndef _H_SAMSON_SYSTEM_KEYVALUE
