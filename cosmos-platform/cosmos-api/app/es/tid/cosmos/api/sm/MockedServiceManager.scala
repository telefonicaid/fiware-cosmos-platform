package es.tid.cosmos.api.sm

import java.util.UUID
import scala.concurrent._
import ExecutionContext.Implicits.global
import scala.collection.mutable

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.ServiceException

/**
 * In-memory, simulated
 * @author sortega
 */
class MockedServiceManager(
    /** Cluster state transition delay in millis */
    transitionDelay: Int
  ) extends ServiceManager {

  private class FakeCluster(override val name: String, override val size: Int,
                            override val id: ClusterId = new ClusterId)
    extends ClusterDescription {

    override def state = currentState
    var currentState: ClusterState = Provisioning

    def completeProvision() {
      if (currentState == Provisioning) currentState = Running
    }

    def completeTermination() {
      if (currentState == Terminating) currentState = Terminated
    }

    defer(transitionDelay, completeProvision())
  }

  private val clusters: mutable.Map[ClusterId, FakeCluster] =
    new mutable.HashMap[ClusterId, FakeCluster]
      with mutable.SynchronizedMap[ClusterId, FakeCluster] {
      val cluster0 = new FakeCluster(
        id = MockedServiceManager.defaultClusterId,
        name = "cluster0", size = 100)
      put(cluster0.id, cluster0)
    }

  def clusterIds: Seq[ClusterId] = clusters.keySet.toSeq

  def createCluster(name: String, clusterSize: Int): ClusterId = {
    val cluster = new FakeCluster(name, clusterSize)
    clusters.put(cluster.id, cluster)
    cluster.id
  }

  def describeCluster(clusterId: ClusterId): Option[ClusterDescription] = clusters.get(clusterId)

  def terminateCluster(id: ClusterId) {
    if (!clusters.contains(id))
      throw new ServiceException("Unknown cluster")
    val cluster = clusters.get(id).get
    cluster.currentState = Terminating
    defer(transitionDelay, cluster.completeTermination())
  }

  private def defer(delay: Int, action: => Unit) {
    future {
      Thread.sleep(delay)
      action
    }
  }
}

object MockedServiceManager {
  val defaultClusterId = new ClusterId(UUID.fromString("00000000-0000-0000-0000-000000000000"))
}