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
#ifndef _H_SAMSON_CLUSTER_NODE_TYPE
#define _H_SAMSON_CLUSTER_NODE_TYPE


namespace samson {
/**
 * Unique packet type sent over the network between workers and delilahs
 */

// Type of elements in the cluster
typedef enum {
  DelilahNode,
  WorkerNode,
  UnknownNode
} ClusterNodeType;

const char *ClusterNodeType2str(ClusterNodeType type);
}

#endif
