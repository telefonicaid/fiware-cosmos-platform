
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
