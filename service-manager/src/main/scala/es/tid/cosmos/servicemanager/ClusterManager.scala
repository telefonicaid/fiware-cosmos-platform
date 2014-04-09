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

package es.tid.cosmos.servicemanager

import scala.concurrent.Future

import es.tid.cosmos.servicemanager.clusters.{ClusterDescription, ImmutableClusterDescription}
import es.tid.cosmos.servicemanager.configuration.{ConfigurationBundle, DynamicPropertiesFactory}

trait ClusterManager {

  def deployCluster(
    clusterDescription: ImmutableClusterDescription,
    services: Seq[AnyServiceInstance],
    dynamicProperties: DynamicPropertiesFactory): Future[ConfigurationBundle]

  def removeCluster(cluster: ClusterDescription): Future[Any]

  def changeServiceConfiguration(
    clusterDescription: ImmutableClusterDescription,
    dynamicProperties: DynamicPropertiesFactory,
    serviceDescription: AnyServiceInstance): Future[Any]
}
