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
import es.tid.cosmos.api.controllers.pages.Pages
import es.tid.cosmos.servicemanager.ServiceManagerComponent
import es.tid.cosmos.api.oauth2.OAuthClientComponent

/**
 * Web application template to be mixed-in with its dependencies.
 */
abstract class Application {
  self: ServiceManagerComponent with OAuthClientComponent =>
  lazy val controllers: Map[Class[Controller], Controller] = {
    val sm = self.serviceManager()
    controllerMap(
      new Pages(self.oAuthClient()),
      new CosmosResource(),
      new ClustersResource(sm),
      new ClusterResource(sm)
    )
  }

  def services: ServiceManagerComponent with OAuthClientComponent = self

  private def controllerMap(controllers: Controller*) = Map(
    (for (controller <- controllers)
      yield controller.getClass.asInstanceOf[Class[Controller]] -> controller): _*)
}
