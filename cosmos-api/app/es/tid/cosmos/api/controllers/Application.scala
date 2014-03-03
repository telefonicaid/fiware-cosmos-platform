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

import es.tid.cosmos.api.auth.multiauth.MultiAuthProviderComponent
import es.tid.cosmos.api.auth.request.RequestAuthenticationComponent
import es.tid.cosmos.api.controllers.admin._
import es.tid.cosmos.api.controllers.admin.stats.StatsResource
import es.tid.cosmos.api.controllers.cluster.ClusterResource
import es.tid.cosmos.api.controllers.cosmos.CosmosResource
import es.tid.cosmos.api.controllers.info.InfoResource
import es.tid.cosmos.api.controllers.pages.{AdminPage, Pages}
import es.tid.cosmos.api.controllers.profile.ProfileResource
import es.tid.cosmos.api.controllers.services.ServicesResource
import es.tid.cosmos.api.controllers.storage.StorageResource
import es.tid.cosmos.api.controllers.task.TaskResource
import es.tid.cosmos.api.profile.CosmosProfileDaoComponent
import es.tid.cosmos.api.task.TaskDaoComponent
import es.tid.cosmos.common.ConfigComponent
import es.tid.cosmos.platform.ial.InfrastructureProviderComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent

/** Web application template to be mixed-in with its dependencies. */
abstract class Application {
  this: ServiceManagerComponent
    with InfrastructureProviderComponent
    with MultiAuthProviderComponent
    with CosmosProfileDaoComponent
    with TaskDaoComponent
    with MaintenanceStatusComponent
    with RequestAuthenticationComponent
    with ConfigComponent =>

  lazy val conf = this.config

  lazy val dao = this.cosmosProfileDao

  lazy val controllers: Map[Class[Controller], Controller] = {
    val status = this.maintenanceStatus
    val sm = this.serviceManager()
    val ial = this.infrastructureProvider
    val multiAuthProvider = this.multiAuthProvider
    val auth = apiRequestAuthentication
    controllerMap(
      // Pages
      new Pages(multiAuthProvider, sm, taskDao, dao, status, conf.getConfig("pages")),
      new AdminPage(dao, status),
      new CliConfigResource(dao),

      // Non-authenticated user API
      new CosmosResource(),
      new ServicesResource(sm),

      // Authenticated user API
      new StatsResource(auth, dao, sm, ial),
      new InfoResource(auth, dao, sm),
      new ProfileResource(auth, dao),
      new ClusterResource(auth, sm, taskDao, dao, status),
      new StorageResource(auth, sm, status),
      new MaintenanceResource(auth, status),
      new TaskResource(auth, taskDao),

      // Admin API
      new UserResource(multiAuthProvider, sm, dao, status)
    )
  }

  def services: ServiceManagerComponent
    with MultiAuthProviderComponent
    with MaintenanceStatusComponent
    with TaskDaoComponent = this

  private def controllerMap(controllers: Controller*) = Map(
    (for (controller <- controllers)
      yield controller.getClass.asInstanceOf[Class[Controller]] -> controller): _*)
}
