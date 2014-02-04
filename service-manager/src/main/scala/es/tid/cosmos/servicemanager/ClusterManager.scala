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

import es.tid.cosmos.servicemanager.clusters.{ClusterId, ImmutableClusterDescription}
import es.tid.cosmos.servicemanager.ambari.DynamicPropertiesFactory
import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription

trait ClusterManager {
  type ServiceDescriptionType

  def deployCluster(
    clusterDescription: ImmutableClusterDescription,
    serviceDescriptions: Seq[AmbariServiceDescription],
    dynamicProperties: DynamicPropertiesFactory): Future[Any]

  def removeCluster(cluster: ImmutableClusterDescription): Future[Any]

  def changeServiceConfiguration(
    id: ClusterId,
    dynamicProperties: DynamicPropertiesFactory,
    clusterDescription: ImmutableClusterDescription,
    serviceDescription: ServiceDescriptionType): Future[Any]
}