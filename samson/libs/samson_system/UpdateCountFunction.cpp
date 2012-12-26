/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) 2012 Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/*
 * FILE            UpdateCountFunction.cpp
 *
 * AUTHOR          Gregorio Escalada
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Definition of UpdateCountFunction class methods to update a time weighted count
 *
 */

#include "samson_system/UpdateCountFunction.h"   // Own interface

#include <math.h>

namespace samson {
namespace system {
double UpdateCountFunction::UpdateCount(double old_count, size_t old_time, size_t new_time) {
  int diff = static_cast <int>(new_time - old_time);

  return (old_count * pow(forgetting_factor_, diff));
}
}
}   // End of namespaces
