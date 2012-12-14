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

#include "NodeIdentifier.h"  // Own interfafe


namespace samson {
NodeIdentifier::NodeIdentifier() {
  node_type = UnknownNode;
  id = static_cast<size_t>(-1);
}

NodeIdentifier::NodeIdentifier(gpb::NodeIdentifier pb_node_identifier) {
  switch (pb_node_identifier.node_type()) {
    case gpb::NodeIdentifier_NodeType_Delilah:
      node_type = DelilahNode;
      break;
    case gpb::NodeIdentifier_NodeType_Worker:
      node_type = WorkerNode;
      break;

    default:
      node_type = UnknownNode;
      break;
  }

  id = pb_node_identifier.id();
}

NodeIdentifier::NodeIdentifier (ClusterNodeType _node_type, size_t _id) {
  node_type = _node_type;
  id = _id;
}

void NodeIdentifier::fill(gpb::NodeIdentifier *pb_node_identifier) {
  switch (node_type) {
    case DelilahNode:
      pb_node_identifier->set_node_type(gpb::NodeIdentifier_NodeType_Delilah);
      break;
    case WorkerNode:
      pb_node_identifier->set_node_type(gpb::NodeIdentifier_NodeType_Worker);
      break;
    case UnknownNode:
      LM_X(1, ("Internal error"));
      break;
  }
  pb_node_identifier->set_id(id);
}

bool NodeIdentifier::operator==(const NodeIdentifier&  other) const {
  if (node_type != other.node_type) {
    return false;
  }
  if (id != other.id) {
    return false;
  }

  return true;
}

std::string NodeIdentifier::str() const {
  if (id == static_cast<size_t>(-1)) {
    return au::str("%s:Unknown", ClusterNodeType2str(node_type));
  }
  if (node_type == DelilahNode) {
    return au::str("%s_%s", ClusterNodeType2str(node_type), au::code64_str(id).c_str());
  }
  return au::str("%s_%lu", ClusterNodeType2str(node_type), id);
}
}