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

package es.tid.cosmos.servicemanager.services.dependencies

import es.tid.cosmos.servicemanager.services._
import es.tid.cosmos.servicemanager._

/** Object expressing inter-service dependencies. */
object ServiceDependencies {

  /** All services registered.
    *
    * Nice-to-have: This could be retrieved by refection once services are organized in only
    * one ambari-independent package.
    */
  val ServiceCatalogue: Set[Service] = Set(
    CosmosUserService,
    HCatalog,
    Hdfs,
    Hive,
    InfinityfsDriver,
    InfinityfsServer,
    MapReduce2,
    Oozie,
    Pig,
    Sqoop,
    WebHCat,
    Yarn,
    Zookeeper
  )

  private val Dependencies = new ServiceDependencyMapping(ServiceCatalogue)

  def executionPlan(serviceInstances: Set[AnyServiceInstance]): Seq[AnyServiceInstance] = {

    /** Use the passed instance, pick a default one or throw */
    def toServiceInstance(service: Service): AnyServiceInstance =
      serviceInstances.find(_.service == service)
        .orElse(service.defaultInstance)
        .getOrElse(throw new RuntimeException(s"No parametrization found for service ${service.name}"))

    val requestedServices = serviceInstances.map(_.service)
    val serviceExecutionPlan = Dependencies.executionPlan(requestedServices)
    serviceExecutionPlan.map(toServiceInstance)
  }
}
