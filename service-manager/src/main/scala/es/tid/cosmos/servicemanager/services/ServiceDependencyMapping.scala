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
