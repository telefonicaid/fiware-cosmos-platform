package es.tid.cosmos.api.controllers

import play.api.mvc.Controller
import es.tid.cosmos.api.sm.ServiceManagerComponent

/**
 * @author sortega
 */
trait Application extends Controller
  with Pages
  with Cosmos
  with Clusters
  with Cluster {
  self: ServiceManagerComponent =>
}
