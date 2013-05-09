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

import es.tid.cosmos.api.controllers.cluster.ClusterResource
import es.tid.cosmos.api.controllers.clusters.ClustersResource
import es.tid.cosmos.api.controllers.cosmos.CosmosResource
import es.tid.cosmos.servicemanager.ServiceManagerComponent

/**
 * Web application template to be mixed-in with its dependencies.
 */
abstract class Application {
  self: ServiceManagerComponent =>
  lazy val controllers: Map[Class[Controller], Controller] = controllerMap(
    new Pages(),
    new CosmosResource(),
    new ClustersResource(self.serviceManager),
    new ClusterResource(self.serviceManager)
  )

  private def controllerMap(controllers: Controller*) = Map(
    (for (controller <- controllers)
      yield controller.getClass.asInstanceOf[Class[Controller]] -> controller): _*)
}
