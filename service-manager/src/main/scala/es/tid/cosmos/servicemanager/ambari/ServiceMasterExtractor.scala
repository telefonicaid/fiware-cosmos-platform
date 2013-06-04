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

package es.tid.cosmos.servicemanager.ambari

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import es.tid.cosmos.servicemanager.ComponentDescription
import es.tid.cosmos.servicemanager.ambari.rest.{Host, Cluster}
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription

/**
 * Extractor that returns the master host for a given service, i.e., the host that contains the
 * master component of that service
 */
object ServiceMasterExtractor {

  def getServiceMaster(cluster: Cluster, description: AmbariServiceDescription): Future[Host] = {
    val masterComponent = findMasterComponent(description)
    for {
      hosts <- cluster.getHosts
    } yield findMasterHost(hosts, masterComponent).getOrElse(throw ServiceMasterNotFound(cluster, description))
  }

  private def findMasterHost(hosts: Seq[Host], masterComponent: ComponentDescription) =
    hosts.find(_.getComponentNames.contains(masterComponent.name))

  private def findMasterComponent(description: AmbariServiceDescription) = {
    val masterComponentOption = description.components.find(_.isMaster)
    require(masterComponentOption != None, "ServiceDescription must contain a master component")
    masterComponentOption.get
  }

  case class ServiceMasterNotFound(
    cluster: Cluster, description: AmbariServiceDescription) extends Exception(
      s"Masternode not found for Service[${description.name}]-Cluster[${cluster.name}]")
}
