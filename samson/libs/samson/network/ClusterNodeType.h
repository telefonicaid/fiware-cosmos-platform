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
