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

import es.tid.cosmos.servicemanager._
import es.tid.cosmos.servicemanager.ambari.AmbariClusterManager
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceFactory
import es.tid.cosmos.servicemanager.clusters.{ClusterDescription, ImmutableClusterDescription}
import es.tid.cosmos.servicemanager.configuration.DynamicPropertiesFactory

class MockAmbariClusterManager extends AmbariClusterManager(
    ambariServer = null,
    rootPrivateSshKey = "",
    configPath = "/tmp/null",
    serviceLookup = AmbariServiceFactory.lookup
  ) {

  override def deployCluster(
    clusterDescription: ImmutableClusterDescription,
    serviceDescriptions: Seq[AnyServiceInstance],
    dynamicProperties: DynamicPropertiesFactory): Future[Unit] = Future.successful()

  override def removeCluster(cluster: ClusterDescription): Future[Unit] = Future.successful()

  override def changeServiceConfiguration(
    clusterDescription: ImmutableClusterDescription,
    dynamicProperties: DynamicPropertiesFactory,
    serviceDescription: AnyServiceInstance): Future[Unit] = Future.successful()
}
