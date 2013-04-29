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

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global
import es.tid.cosmos.servicemanager.ambari.{Service, Host, Cluster}

trait ServiceDescription {
  val name: String
  val components: Seq[ComponentDescription]
  final def createService(cluster: Cluster, master: Host, slaves: Seq[Host]): Future[Service] = {
    val configurationFuture = applyConfiguration(cluster, master)
    for {
      service <- cluster.addService(name)
      _ <- configurationFuture
      _ <- Future.sequence(components.map(component => service.addComponent(component.name)))
      _ <- master.addComponents(components.filter(_.isMaster).map(_.name): _*)
      _ <- Future.sequence(slaves.map(slave => slave.addComponents(components.filter(!_.isMaster).map(_.name): _*)))
    } yield service
  }

  protected def applyConfiguration(cluster: Cluster, master: Host): Future[Unit]
}
