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

  NodeIdentifier(ClusterNodeType node_type, size_t id);
  NodeIdentifier(const NodeIdentifier& node_identifier);
  NodeIdentifier(gpb::NodeIdentifier pb_node_identifier);
  NodeIdentifier();

  /**
   * \brief Set content manually
   */
  void Set(ClusterNodeType node_type, size_t id) {
    node_type_ = node_type;
    id_ = id;
  }

  /**
   * \brief Copy content from another instance
   */
  void Set(const NodeIdentifier& node_identifier) {
    node_type_ = node_identifier.node_type_;
    id_ = node_identifier.id_;
  }

  /**
   * \brief Fill a GoogleProtocolBuffer structure
   */
  void fill(gpb::NodeIdentifier *pb_node_identifier);

  /**
   * \brief Operator to check equal
   */
  bool operator==(const NodeIdentifier&  other) const;

  /**
   * \brief Operator = to be able to do a=b;
   */
  NodeIdentifier& operator=(const NodeIdentifier& node_identifier) {
    Set(node_identifier);
    return *this;
  }

  /**
   * \brief Operator < to be used in maps
   */
  bool operator<(const NodeIdentifier&  other) const {
    if (node_type_ != other.node_type_) {
      return node_type_ < other.node_type_;
    }
    return id_ < other.id_;
  }

  /**
   * \brief Debug string to show on screen
   */
  std::string str() const;

  ClusterNodeType node_type() const {
    return node_type_;
  };

  size_t id() const {
    return id_;
  };

private:

  ClusterNodeType node_type_;     /**< Node type (worker,delilah,unknown) */
  size_t id_;     /**< worker or delilah identifier */
};
}

#endif  // ifndef _H_SAMSON_NodeIdentifier
