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

package es.tid.cosmos.servicemanager

import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDetails

/** Representation of a service definition.
  *
  * Derived classes should be objects as only an instance per service is assumed.
  */
trait Service {
  /** Type of the values that parametrize it to have a service instance */
  type Parametrization

  val name: String

  /** Direct service dependencies */
  val dependencies: Set[Service] = Set.empty

  /** TODO: replace this method by a factory method  in ambari-service-manager
    * (inverting the dependency) */
  def ambariService: AmbariServiceDetails
}
