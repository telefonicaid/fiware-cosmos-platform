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

#include "au/containers/StringVector.h"
#include "au/tables/Table.h"
#include "au/utils.h"

#include "samson/common/Logs.h"
#include "samson/common/ports.h"
#include "samson/network/NetworkConnection.h"

#include "DelilahNetwork.h"  // Own interface

namespace samson {
DelilahNetwork::DelilahNetwork(std::string connection_type, size_t delilah_id)
  : CommonNetwork(NodeIdentifier(DelilahNode, delilah_id)) {
  LOG_V(logs.network_connection, ("DelilahNetwork %s", au::code64_str(delilah_id).c_str()));
  // Save connection type string  to be send in all hello messages
  connection_type_ = connection_type;
}
}
