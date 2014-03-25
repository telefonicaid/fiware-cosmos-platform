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

/** Representation of the HDFS service. */
object Hdfs extends Service {

  private val umaskPattern = "[0-7]{3}"

  case class HdfsParameters(umask: String) {
    require(umask.matches(umaskPattern), s"Not a valid umask: '$umask'")
  }

  override val name: String = "HDFS"
  override type Parametrization = HdfsParameters
  override val defaultParametrization: Option[HdfsParameters] = Some(HdfsParameters("022"))
  override val dependencies: Set[Service] = Set(Zookeeper, InfinityDriver)
}
