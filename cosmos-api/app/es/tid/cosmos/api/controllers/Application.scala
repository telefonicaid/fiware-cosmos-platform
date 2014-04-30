/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.api.controllers

import play.api.mvc.Controller

import es.tid.cosmos.api.auth.multiauth.MultiAuthProviderComponent
import es.tid.cosmos.api.auth.request.RequestAuthenticationComponent
import es.tid.cosmos.api.email.EmailerComponent
import es.tid.cosmos.api.controllers.admin._
import es.tid.cosmos.api.controllers.admin.stats.StatsResource
import es.tid.cosmos.api.controllers.cluster.ClusterResource
import es.tid.cosmos.api.controllers.cosmos.CosmosResource
import es.tid.cosmos.api.controllers.infinity.{GroupMapperResource, InfinityAuthenticationResource}
import es.tid.cosmos.api.controllers.info.InfoResource
import es.tid.cosmos.api.controllers.pages.{AdminPage, Pages}
import es.tid.cosmos.api.controllers.profile.ProfileResource
import es.tid.cosmos.api.controllers.services.ServicesResource
import es.tid.cosmos.api.controllers.storage.StorageResource
import es.tid.cosmos.api.controllers.task.TaskResource
import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent
import es.tid.cosmos.api.report.ClusterReporter
import es.tid.cosmos.api.task.TaskDaoComponent
import es.tid.cosmos.api.usage.MachineUsageComponent
import es.tid.cosmos.common.ConfigComponent
import es.tid.cosmos.platform.ial.InfrastructureProviderComponent
import es.tid.cosmos.servicemanager.ServiceManagerComponent

/** Web application template to be mixed-in with its dependencies. */
abstract class Application {
  this: ServiceManagerComponent
    with InfrastructureProviderComponent
    with MultiAuthProviderComponent
    with CosmosDataStoreComponent
    with MachineUsageComponent
    with TaskDaoComponent
    with MaintenanceStatusComponent
    with RequestAuthenticationComponent
    with EmailerComponent
    with ConfigComponent =>

  lazy val conf = this.config
  lazy val reporter = new ClusterReporter(
    conf.getString("email.environment"),
    conf.getString("email.fromHost"),
    conf.getString("email.reportToAddress"),
    emailer
  )

  lazy val controllers: Map[Class[Controller], Controller] = {
    val status = this.maintenanceStatus
    val ial = this.infrastructureProvider
    val multiAuthProvider = this.multiAuthProvider
    val auth = apiRequestAuthentication
    controllerMap(
      // Pages
      new Pages(
        multiAuthProvider,
        serviceManager,
        reporter,
        taskDao,
        store,
        status,
        conf.getConfig("pages")),
      new AdminPage(store, status),
      new CliConfigResource(store),

      // Non-authenticated user API
      new CosmosResource(),
      new ServicesResource(serviceManager),

      // Authenticated user API
      new StatsResource(auth, store, serviceManager, ial),
      new InfoResource(auth, store, serviceManager, machineUsage),
      new ProfileResource(auth, store),
      new ClusterResource(auth, serviceManager, machineUsage, taskDao, store, status, reporter),
      new StorageResource(auth, serviceManager, status),
      new MaintenanceResource(auth, status),
      new TaskResource(auth, taskDao),

      // Infinity API
      new InfinityAuthenticationResource(store, serviceManager, conf),
      new GroupMapperResource(store, conf),

      // Admin API
      new UserResource(multiAuthProvider, serviceManager, store, status, reporter)
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
