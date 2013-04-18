package es.tid.cosmos.sm.ambari

import es.tid.cosmos.sm._

/**
 * Placeholder implementation of the ServiceManager trait
 *
 * @author sortega
 */
class AmbariServiceManager extends ServiceManager {

  val unsupportedMethod = ServiceException("Not yet supported")

  def clusterIds: Seq[_root_.es.tid.cosmos.sm.ClusterId] = throw unsupportedMethod

  def createCluster(name: String, clusterSize: Int): _root_.es.tid.cosmos.sm.ClusterId = throw unsupportedMethod

  def describeCluster(id: _root_.es.tid.cosmos.sm.ClusterId): Option[ClusterDescription] = throw unsupportedMethod

  def terminateCluster(id: _root_.es.tid.cosmos.sm.ClusterId) { throw unsupportedMethod }
}
