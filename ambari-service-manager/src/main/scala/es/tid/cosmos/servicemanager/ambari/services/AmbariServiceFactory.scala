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

package es.tid.cosmos.servicemanager.ambari.services

import es.tid.cosmos.servicemanager.services.Service

/** Factory of ambari services.
  *
  * This can be improved by using reflection on this package.
  */
private[ambari] object AmbariServiceFactory {

  private val ambariServices: Set[AmbariService] = Set(
    AmbariCosmosUserService,
    AmbariHCatalog,
    AmbariHdfs,
    AmbariHive,
    AmbariInfinityDriver,
    AmbariInfinityServer,
    AmbariMapReduce2,
    AmbariOozie,
    AmbariPig,
    AmbariSqoop,
    AmbariWebHCat,
    AmbariYarn,
    AmbariZookeeper
  )

  private val mapping: Map[Service, AmbariService] = (for {
    ambariService <- ambariServices
  } yield ambariService.service -> ambariService).toMap

  def lookup(service: Service): AmbariService =
    mapping.getOrElse(service, throw new IllegalArgumentException(
      s"Unsupported service $service: no mapping to Ambari services"
    ))
}
