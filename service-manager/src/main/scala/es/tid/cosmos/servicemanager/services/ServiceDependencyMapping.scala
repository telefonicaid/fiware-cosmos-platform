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

package es.tid.cosmos.servicemanager.services

import es.tid.cosmos.servicemanager.services.dependencies.{DependencyMapping, DependencyType}

class ServiceDependencyMapping(catalogue: Set[Service])
  extends DependencyMapping[Service](ServiceDependencyMapping.dependencyMap(catalogue)) {

  /** Given a set services return an execution plan that consider transitive dependencies and
    * execution order.
    *
    * @param services  Services that want to be installed
    * @return          Input services and their dependencies in a valid installation order
    */
  def executionPlan(services: Set[Service]): Seq[Service] = executionOrder(resolve(services.toSet))
}

object ServiceDependencyMapping {

  def dependencyMap(catalogue: Set[Service]): Map[Service, Map[Service, DependencyType]] = (for {
    service <- catalogue
  } yield service -> service.dependencies.mapping).toMap
}
