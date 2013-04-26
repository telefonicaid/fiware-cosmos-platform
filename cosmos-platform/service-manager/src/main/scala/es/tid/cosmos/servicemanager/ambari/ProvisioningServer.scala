package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.Future
import net.liftweb.json.JsonAST.JValue

/**
 * Representation of a server capable of provisioning clusters.
 *
 * @author adamos
 */
trait ProvisioningServer {
  def listClusterNames: Future[Seq[String]]
  def getCluster(name: String): Future[Cluster]
  def createCluster(name: String, version: String): Future[Cluster]
  def removeCluster(name: String): Future[JValue]
}
