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

import es.tid.cosmos.servicemanager.ServiceInstance
import es.tid.cosmos.servicemanager.services.dependencies.ServiceDependencies

/** Representation of a service definition.
  *
  * Derived classes should be objects as only an instance per service is assumed.
  */
trait Service {
  /** Type of the values that parametrize it to have a service instance */
  type Parametrization

  val name: String

  /** Default parametrization of the service.
    *
    * Services returning a value different from None can be instantiate with default configuration.
    * That is specially useful when instantiating services because they are transitive dependencies
    * of other services.
    */
  def defaultParametrization: Option[Parametrization] = None

  /** Direct service dependencies */
  val dependencies: ServiceDependencies = ServiceDependencies.none

  def instance(parametrization: Parametrization): ServiceInstance[this.type] =
    ServiceInstance[this.type](this, parametrization)

  lazy val defaultInstance: Option[ServiceInstance[this.type]] =
    defaultParametrization.map(instance)
}
