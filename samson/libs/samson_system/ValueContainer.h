
#ifndef _H_SAMSON_SYSTEM_VALUE_CONTAINER
#define _H_SAMSON_SYSTEM_VALUE_CONTAINER

#include "samson_system/Value.h"

namespace samson {
namespace system {
/*
 *
 * ValueContainer
 * Handy class to get a Value instance in the constructor and release it at destructor
 *
 */

class ValueContainer {
  public:

    Value *value; // Instance of value retained in this class

    ValueContainer() {
      value = Value::getInstance();
    }

    ~ValueContainer() {
      Value::reuseInstance(value);
    }
};
}
} // End of namespace

#endif  // ifndef _H_SAMSON_SYSTEM_VALUE_CONTAINER
