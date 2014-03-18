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

import es.tid.cosmos.servicemanager.ServiceInstance

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
  val dependencies: Set[Service] = Set.empty

  def instance(parametrization: Parametrization): ServiceInstance[this.type] =
    ServiceInstance[this.type](this, parametrization)

  lazy val defaultInstance: Option[ServiceInstance[this.type]] =
    defaultParametrization.map(instance)
}
