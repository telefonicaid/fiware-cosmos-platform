package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.Future

/**
 * TODO: Insert description here
 *
 * @author adamos
 */
trait ProvisioningServer {
  def listClusterNames: Future[Seq[String]]
  def getCluster(name: String): Future[Cluster]
  def createCluster(name: String, version: String): Future[Cluster]
  def removeCluster(name: String): Unit
}
