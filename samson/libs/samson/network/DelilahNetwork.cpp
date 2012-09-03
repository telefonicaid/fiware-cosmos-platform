
#include "au/containers/StringVector.h"
#include "au/tables/Table.h"
#include "au/utils.h"

#include "samson/common/ports.h"
#include "samson/network/NetworkConnection.h"

#include "DelilahNetwork.h"  // Own interface

namespace samson {
DelilahNetwork::DelilahNetwork(std::string connection_type, size_t delilah_id)
  : CommonNetwork(NodeIdentifier(DelilahNode, delilah_id)) {
  LM_V(("DelilahNetwork %s", au::code64_str(delilah_id).c_str()));

  // Save connection type string  to be send in all hello messages
  connection_type_ = connection_type;
}
}
