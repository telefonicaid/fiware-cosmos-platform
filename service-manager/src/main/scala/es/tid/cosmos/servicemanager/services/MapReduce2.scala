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

import es.tid.cosmos.servicemanager.services.dependencies.ServiceDependencies

/** Representation of the Map Reduce 2 service.
  *
  * The service needs to run along with YARN.
  * @see [[Yarn]]
  */
object MapReduce2 extends Service with NoParametrization {
  override val name: String = "MAPREDUCE2"
  override val dependencies = ServiceDependencies.required(Yarn)
}
