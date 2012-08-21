
#ifndef _H_SAMSON_SYSTEM_KEYVALUE
#define _H_SAMSON_SYSTEM_KEYVALUE

#include "au/StringComponents.h"
#include "au/Tokenizer.h"
#include "au/containers/vector.h"
#include "au/string.h"

#include "samson_system/Value.h"
#include <samson/module/samson.h>


namespace samson {
namespace system {
// -----------------------------------------------------------------------
// KeyValue : system.Value system.Value
// -----------------------------------------------------------------------

class KeyValue {
public:

  samson::system::Value *key;
  samson::system::Value *value;

  // Constructor
  KeyValue() {
    key = NULL;
    value = NULL;
  }

  KeyValue(samson::system::Value *_key, samson::system::Value *_value);
};
}
}

#endif  // ifndef _H_SAMSON_SYSTEM_KEYVALUE
