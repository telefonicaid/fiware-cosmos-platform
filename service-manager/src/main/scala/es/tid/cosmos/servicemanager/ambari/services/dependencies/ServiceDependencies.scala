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

package es.tid.cosmos.servicemanager.ambari.services.dependencies

import es.tid.cosmos.servicemanager.{ServiceInstance, NoParametrization, AnyServiceInstance, Service}
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.services._
import es.tid.cosmos.servicemanager.ServiceInstance

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

  /** Default instances of services. At the moment, just services with no parametrization and
    * services that have sensible default parametrisation.
    */
  val DefaultServiceInstances: Set[AnyServiceInstance] = ServiceCatalogue.collect {
    case service: Service with NoParametrization => service.instance
    case CosmosUserService => ServiceInstance[CosmosUserService](
      new CosmosUserService(users = Seq.empty),
      Seq.empty
    )
  }

  private val Dependencies = new ServiceDependencyMapping(ServiceCatalogue)

  def executionPlan(serviceInstances: Set[AnyServiceInstance]): Seq[AnyServiceInstance] = {
    val requestedServices = serviceInstances.map(_.service)
    val serviceExecutionPlan = Dependencies.executionPlan(requestedServices)
    val instanceMap = mapByService(DefaultServiceInstances) ++ mapByService(serviceInstances)
    serviceExecutionPlan.map { service =>
      instanceMap.getOrElse(service,
        throw new RuntimeException(s"No parametrization found for service ${service.name}"))
    }
  }

  private def mapByService(instances: Set[AnyServiceInstance]): Map[Service, AnyServiceInstance] =
    instances.map(instance => (instance.service, instance)).toMap
}
