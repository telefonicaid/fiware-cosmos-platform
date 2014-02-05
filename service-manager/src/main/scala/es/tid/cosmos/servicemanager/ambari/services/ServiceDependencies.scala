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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.ServiceDescription

/** Object expressing inter-service dependencies. */
object ServiceDependencies {
  type Service = ServiceDescription

  private val Dependencies: Map[Service, Seq[Service]] = Map(
    Hdfs -> Seq(Zookeeper),
    MapReduce2 -> Seq(Yarn),
    Hive -> Seq(HCatalog, WebHCat)
  )

  implicit class ServiceBundle(services: Seq[Service]) {

    /** Extend a given collection of services with their dependencies.
      * The dependencies of a service will be added before that service in the resulting collection
      * so as to enforce an order where a service's dependencies will be processed first. e.g.
      *
      * {{{
      *   // assume that serviceA depends on serviceC and serviceB on serviceD
      *   import ServiceDependencies._
      *
      *   Seq(serviceA, serviceB).withDepdenencies == Seq(serviceC, serviceA, serviceD, serviceB)
      * }}}
      */
    val withDependencies: Seq[Service] =
      services.flatMap(service => Dependencies.getOrElse(service, Seq.empty) :+ service)
  }
}
