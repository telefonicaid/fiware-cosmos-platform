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

/** Mix-in for services that don't need parametrization */
trait NoParametrization { this: Service =>
  override type Parametrization = Unit
  override lazy val defaultParametrization: Option[Parametrization] = Some(())
  lazy val instance: ServiceInstance[this.type] = defaultInstance.get
}
