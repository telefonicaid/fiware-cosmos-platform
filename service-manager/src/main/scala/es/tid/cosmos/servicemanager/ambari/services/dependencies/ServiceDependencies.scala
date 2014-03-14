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

import es.tid.cosmos.servicemanager.Service
import es.tid.cosmos.servicemanager.ambari.services._
import es.tid.cosmos.servicemanager.services.ServiceDependencyMapping

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

  class ServiceBundle(services: Seq[Service]) {

    /** Extend a given collection of services with their dependencies.
      * The dependencies of a service will be added before that service in the resulting collection
      * so as to enforce an order where a service's dependencies will be processed first. e.g.
      *
      * {{{
      *   // assume that serviceA depends on serviceC and serviceB on serviceD
      *   import ServiceDependencies._
      *
      *   Seq(serviceA, serviceB).withDependencies == Seq(serviceC, serviceA, serviceD, serviceB)
      * }}}
      */
    val withDependencies: Seq[Service] = Dependencies.executionPlan(services.toSet)
  }
}
