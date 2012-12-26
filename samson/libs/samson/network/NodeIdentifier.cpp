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
  node_type_ = UnknownNode;
  id_ = SIZE_T_UNDEFINED;
}

NodeIdentifier::NodeIdentifier(gpb::NodeIdentifier pb_node_identifier) {
  switch (pb_node_identifier.node_type()) {
    case gpb::NodeIdentifier_NodeType_Delilah:
      node_type_ = DelilahNode;
      break;
    case gpb::NodeIdentifier_NodeType_Worker:
      node_type_ = WorkerNode;
      break;

    default:
      node_type_ = UnknownNode;
      break;
  }

  id_ = pb_node_identifier.id();
}

NodeIdentifier::NodeIdentifier(const NodeIdentifier& node_identifier) {
  Set(node_identifier);
}

NodeIdentifier::NodeIdentifier (ClusterNodeType node_type, size_t id) {
  node_type_ = node_type;
  id_ = id;
}

void NodeIdentifier::fill(gpb::NodeIdentifier *pb_node_identifier) {
  switch (node_type_) {
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
  pb_node_identifier->set_id(id_);
}

bool NodeIdentifier::operator==(const NodeIdentifier&  other) const {
  if (node_type_ != other.node_type_) {
    return false;
  }
  if (id_ != other.id_) {
    return false;
  }

  return true;
}

std::string NodeIdentifier::str() const {
  switch (node_type_) {
    case UnknownNode:
      return au::str("%s:Unknown", ClusterNodeType2str(node_type_));

      break;

    case WorkerNode:
      return au::str("%s_%lu", ClusterNodeType2str(node_type_), id_);

      break;

    case DelilahNode:
      return au::str("%s_%s", ClusterNodeType2str(node_type_), au::code64_str(id_).c_str());

      break;
  }

  return au::str("%s:Unknown", ClusterNodeType2str(node_type_));
}
}