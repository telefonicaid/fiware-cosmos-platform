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

#ifndef _H_ENVIRONMENT_OPERATIONS
#define _H_ENVIRONMENT_OPERATIONS


#include "samson/common/coding.h"                     // KVInfo
#include "samson/common/samson.pb.h"                  // samson::network::...
#include "samson/module/Environment.h"                // samson::Environment

namespace samson {
void copyEnviroment(Environment *from, gpb::Environment *to);
void copyEnviroment(const gpb::Environment & from, Environment *to);
}

#endif
