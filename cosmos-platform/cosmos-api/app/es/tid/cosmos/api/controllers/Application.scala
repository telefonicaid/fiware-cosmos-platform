package es.tid.cosmos.api.controllers

import play.api.mvc.Controller
import es.tid.cosmos.api.controllers.clusters.ClustersResource
import es.tid.cosmos.servicemanager.ServiceManagerComponent

/**
 * @author sortega
 */
trait Application extends Controller
  with Pages
  with CosmosResource
  with ClustersResource
  with ClusterResource {
  self: ServiceManagerComponent =>
}
