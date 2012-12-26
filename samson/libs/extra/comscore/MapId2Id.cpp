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


#include "MapId2Id.h"  // Own interface

namespace samson {
namespace comscore {
bool compareId2Id(const Id2Id&a, const Id2Id&b) {
  if (a.first != b.first) {
    return ( a.first < b.first );
  }
  return ( a.second < b.second );
}
}
}
