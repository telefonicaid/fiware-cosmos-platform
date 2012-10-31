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




#ifndef _H_SAMSON_DELILAH_NETWORK
#define _H_SAMSON_DELILAH_NETWORK

#include "engine/EngineElement.h"

#include "samson/network/CommonNetwork.h"
#include "samson/network/NetworkInterface.h"

namespace samson {
class DelilahNetwork : public CommonNetwork {
  // String describing the type of delilah connection ( console, push, pop, streamConnector, etc... )
  std::string connection_type_;

public:

  // Constructor & Destructor
  DelilahNetwork(std::string _connection_type, size_t delilah_id);
  ~DelilahNetwork() {
  }
};
}

#endif  // ifndef _H_SAMSON_DELILAH_NETWORK
