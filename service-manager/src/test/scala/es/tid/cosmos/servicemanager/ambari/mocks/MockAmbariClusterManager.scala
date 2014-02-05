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

package es.tid.cosmos.servicemanager.ambari.mocks

import scala.concurrent.Future

import es.tid.cosmos.servicemanager.clusters.ImmutableClusterDescription
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription
import es.tid.cosmos.servicemanager.ambari.{DynamicPropertiesFactory, AmbariClusterManager}

class MockAmbariClusterManager extends AmbariClusterManager(null, "") {

  override def deployCluster(
    clusterDescription: ImmutableClusterDescription,
    serviceDescriptions: Seq[AmbariServiceDescription],
    dynamicProperties: DynamicPropertiesFactory): Future[Unit] = Future.successful()

  override def removeCluster(cluster: ImmutableClusterDescription): Future[Unit] = Future.successful()

  override def changeServiceConfiguration(
    clusterDescription: ImmutableClusterDescription,
    dynamicProperties: DynamicPropertiesFactory,
    serviceDescription: AmbariServiceDescription): Future[Unit] = Future.successful()
}
