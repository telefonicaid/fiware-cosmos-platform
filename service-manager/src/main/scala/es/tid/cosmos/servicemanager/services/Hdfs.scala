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
import es.tid.cosmos.servicemanager.ambari.services.{AmbariHdfs, AmbariServiceDetails}

/** Representation of the HDFS service. */
object Hdfs extends Service with NoParametrization {
  override val name: String = "HDFS"
  override val dependencies: Set[Service] = Set(Zookeeper, InfinityfsDriver)
  override def ambariService: AmbariServiceDetails = AmbariHdfs
}
