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
import es.tid.cosmos.api.controllers.profile.ProfileResource
import es.tid.cosmos.api.controllers.storage.StorageResource
import es.tid.cosmos.api.oauth2.OAuthClientComponent
import es.tid.cosmos.api.profile.CosmosProfileDaoComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent

/**
 * Web application template to be mixed-in with its dependencies.
 */
abstract class Application {
  this: ServiceManagerComponent with OAuthClientComponent with CosmosProfileDaoComponent =>

  lazy val dao = this.cosmosProfileDao

  lazy val controllers: Map[Class[Controller], Controller] = {
    val sm = this.serviceManager()
    controllerMap(
      new Pages(this.oAuthClient(), sm, dao),
      new CosmosResource(),
      new ProfileResource(dao),
      new ClustersResource(sm, dao),
      new ClusterResource(sm, dao),
      new StorageResource(sm, dao)
    )
  }

  def services: ServiceManagerComponent with OAuthClientComponent = this

  private def controllerMap(controllers: Controller*) = Map(
    (for (controller <- controllers)
      yield controller.getClass.asInstanceOf[Class[Controller]] -> controller): _*)
}
