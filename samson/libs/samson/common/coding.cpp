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

#include "samson/common/coding.h"
#include "samson/module/ModulesManager.h"

namespace samson {
// Operations to compare formats
bool operator==(const KVFormat & left, const KVFormat & right) {
  if (left.keyFormat != right.keyFormat) {
    return false;
  }
  if (left.valueFormat != right.valueFormat) {
    return false;
  }
  return true;
}

bool operator!=(const KVFormat & left, const KVFormat & right) {
  if (left.keyFormat != right.keyFormat) {
    return true;
  }
  if (left.valueFormat != right.valueFormat) {
    return true;
  }
  return false;
}
}

