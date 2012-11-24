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

  ClusterNodeType node_type;
  size_t id;

  NodeIdentifier();
  NodeIdentifier(gpb::NodeIdentifier pb_node_identifier);
  NodeIdentifier(ClusterNodeType _node_type, size_t _id);
  NodeIdentifier(const std::string& name);

  void fill(gpb::NodeIdentifier *pb_node_identifier);

  bool operator==(const NodeIdentifier&  other);

  std::string str();

  std::string getCodeName() const;

  bool isDelilahOrUnknown();
};
}

#endif  // ifndef _H_SAMSON_NodeIdentifier