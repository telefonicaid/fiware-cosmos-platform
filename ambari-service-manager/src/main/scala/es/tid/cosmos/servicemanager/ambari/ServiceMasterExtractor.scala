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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import es.tid.cosmos.servicemanager.ambari.rest.{Host, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.{ComponentDescription, AmbariService}

/** Extractor that returns the master host for a given service, i.e., the host that contains the
  * master component of that service
  */
object ServiceMasterExtractor {

  def getServiceMaster(cluster: Cluster, service: AmbariService): Future[Host] = {
    val masterComponent = findMasterComponent(service)
    for (hosts <- cluster.getHosts) yield findMasterHost(hosts, masterComponent)
      .getOrElse(throw ServiceMasterNotFound(cluster, service))
  }

  private def findMasterHost(hosts: Seq[Host], masterComponent: ComponentDescription) =
    hosts.find(_.getComponentNames.contains(masterComponent.name))

  private def findMasterComponent(service: AmbariService) = {
    val masterComponentOption = service.components.find(_.isMaster)
    require(masterComponentOption != None, "ServiceDescription must contain a master component")
    masterComponentOption.get
  }

  case class ServiceMasterNotFound(cluster: Cluster, description: AmbariService)
    extends Exception(
      s"Masternode not found for Service[${description.service.name}]-Cluster[${cluster.name}]")
}
