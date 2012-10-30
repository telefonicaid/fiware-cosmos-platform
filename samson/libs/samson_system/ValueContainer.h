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
    Value *value;   // Instance of value retained in this class

    ValueContainer() {
      value = Value::getInstance();
    }

    ~ValueContainer() {
      Value::reuseInstance(value);
    }
};
}
}   // End of namespace

#endif  // ifndef _H_SAMSON_SYSTEM_VALUE_CONTAINER
