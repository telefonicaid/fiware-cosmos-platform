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

import es.tid.cosmos.servicemanager.services.dependencies.ServiceDependencies

/** Representation of the HDFS service. */
object Hdfs extends Service {

  private val umaskPattern = "[0-7]{3}"

  case class HdfsParameters(umask: String) {
    require(umask.matches(umaskPattern), s"Not a valid umask: '$umask'")
  }

  override val name: String = "HDFS"
  override type Parametrization = HdfsParameters
  override val defaultParametrization: Option[HdfsParameters] = Some(HdfsParameters("022"))
  override val dependencies =
    ServiceDependencies.required(Zookeeper, InfinityDriver).optional(InfinityServer)
}
