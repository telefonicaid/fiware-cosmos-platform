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

package es.tid.cosmos.servicemanager.ambari.services

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import es.tid.cosmos.servicemanager.{ComponentDescription, ServiceDescription}
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationContributor
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Host, Cluster}

/**
 * Representation of a service definition that allows service instantiation for a given Ambari
 * cluster. The service definition is also capable of contributing to the cluster configuration.
 *
 * @see ConfigurationContributor
 * @see ServiceDescription
 */
private[ambari] trait AmbariServiceDescription extends ConfigurationContributor with ServiceDescription {
  /**
   * Create a service instance on a given cluster.
   *
   * @param cluster the cluster for which to instantiate the service
   * @param master the cluster's master node
   * @param slaves the cluster's slave nodes
   * @return the future of the service instance for the given cluster
   */
  def createService(cluster: Cluster, master: Host, slaves: Seq[Host]): Future[Service] =
    for {
      service <- cluster.addService(name)
      _ <- Future.sequence(components.map(component => service.addComponent(component.name)))
      _ <- master.addComponents(components.filter(_.isMaster).map(_.name))
      (masterAndSlaveHost, exclusiveSlaves) = slaves.partition(_.name == master.name)
      _ <- addComponents(exclusiveSlaves, _.isSlave)
      _ <- addComponents(masterAndSlaveHost, c => c.isSlave && !c.isMaster)
    } yield service

  private def addComponents(hosts: Seq[Host], componentFilter: ComponentDescription => Boolean) =
    Future.sequence(
      hosts.map(_.addComponents(components.filter(componentFilter).map(_.name))))

  /**
   * The state of a service to be considered as running.
   */
  def runningState: ServiceState =
    if (components.forall(_.isClient)) InstalledService else StartedService
}
