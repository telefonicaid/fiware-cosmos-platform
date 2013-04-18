package es.tid.cosmos

/**
 * @author sortega
 */
package object sm {

  type ClusterId = String

  sealed trait ClusterState {
    val name: String
    val descLine: String
  }
  case object Provisioning extends ClusterState {
    val name: String = "provisioning"
    val descLine: String = "Cluster is acquiring and configuring resources"
  }
  case object Running extends ClusterState {
    val name: String = "running"
    val descLine: String = "Cluster is ready"
  }
  case object Terminating extends ClusterState {
    val name: String = "terminating"
    val descLine: String = "Releasing cluster resources"
  }
  case object Terminated extends ClusterState {
    val name: String = "terminated"
    val descLine: String = "The cluster no longer exists"
  }

  trait ClusterDescription {
    def id: ClusterId
    def name: String
    def size: Int
    def state: ClusterState
  }

  case class ServiceException(message: String, cause: Throwable = null) extends RuntimeException

  trait ServiceManager {
    def clusterIds: Seq[ClusterId]
    def createCluster(name: String, clusterSize: Int): ClusterId
    def describeCluster(id: ClusterId): Option[ClusterDescription]
    def terminateCluster(id: ClusterId): Unit
  }
}
