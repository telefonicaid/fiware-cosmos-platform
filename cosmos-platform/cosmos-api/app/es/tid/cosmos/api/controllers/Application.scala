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
