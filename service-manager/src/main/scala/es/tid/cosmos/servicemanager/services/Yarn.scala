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

import es.tid.cosmos.servicemanager.{NoParametrization, Service}
import es.tid.cosmos.servicemanager.ambari.services.{AmbariYarn, AmbariServiceDetails}

/** Representation of the YARN service. */
object Yarn extends Service with NoParametrization {
  override val name: String = "YARN"
  override val dependencies: Set[Service] = Set(InfinityfsDriver)
  override def ambariService: AmbariServiceDetails = AmbariYarn
}
