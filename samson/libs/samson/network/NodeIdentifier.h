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
#ifndef _H_SAMSON_NodeIdentifier
#define _H_SAMSON_NodeIdentifier


#include "logMsg/logMsg.h"                       // LM_TODO()

#include "au/Status.h"
#include "au/network/FileDescriptor.h"
#include "au/utils.h"

#include "samson/common/EnvironmentOperations.h"  // str( gpb::Message* )
// google protocol buffers

#include "Message.h"                             // samson::MessageType
#include "engine/Buffer.h"                       // engine::Buffer
#include "engine/Engine.h"                       // engine::Engine
#include "engine/MemoryManager.h"                // MemoryManager
#include "engine/NotificationListener.h"

#include "samson/network/ClusterNodeType.h"

namespace samson {
class NodeIdentifier {
public:

  NodeIdentifier(ClusterNodeType _node_type, size_t _id);
  NodeIdentifier(gpb::NodeIdentifier pb_node_identifier);
  NodeIdentifier();

  /**
   * \brief Fill a GoogleProtocolBuffer structure
   */
  void fill(gpb::NodeIdentifier *pb_node_identifier);

  /**
   * \brief Operator to check equal
   */
  bool operator==(const NodeIdentifier&  other) const;

  /**
   * \brief Operator < to be used in maps
   */
  bool operator<(const NodeIdentifier&  other) const {
    if (node_type != other.node_type) {
      return node_type < other.node_type;
    }
    return id < other.id;
  }

  /**
   * \brief Debug string to show on screen
   */
  std::string str() const;

  ClusterNodeType node_type;     /**< Node type (worker,delilah,unknown) */
  size_t id;     /**< worker or delilah identifier */
};
}

#endif  // ifndef _H_SAMSON_NodeIdentifier