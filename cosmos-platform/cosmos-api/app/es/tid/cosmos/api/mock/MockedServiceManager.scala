package es.tid.cosmos.api.mock

import java.util.UUID
import scala.concurrent._
import ExecutionContext.Implicits.global
import scala.collection.mutable

import es.tid.cosmos.sm._

/**
 * In-memory, simulated
 * @author sortega
 */
class MockedServiceManager(
    /** Cluster state transition delay in millis */
    transitionDelay: Int
  ) extends ServiceManager {

  val defaultClusterId = "UUID"

  private class FakeCluster(override val name: String, override val size: Int,
                            override val id: String = UUID.randomUUID().toString)
    extends ClusterDescription {
    override def state = currentState
    var currentState: ClusterState = Provisioning
    def completeProvision() {
      if (currentState == Provisioning) currentState = Running
    }

    def completeTermination() {
      if (currentState == Terminating) currentState = Terminated
    }
  }

  private val clusters: mutable.Map[ClusterId, FakeCluster] =
    new mutable.HashMap[ClusterId, FakeCluster]
      with mutable.SynchronizedMap[ClusterId, FakeCluster] {
      val cluster0 = new FakeCluster(id = defaultClusterId, name = "cluster0", size = 100)
      put(cluster0.id, cluster0)
    }

  def clusterIds: Seq[ClusterId] = clusters.keySet.toSeq

  def createCluster(name: String, clusterSize: Int): ClusterId = {
    val cluster = new FakeCluster(name, clusterSize)
    clusters.put(cluster.id, cluster)
    defer(transitionDelay, cluster.completeProvision())
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
