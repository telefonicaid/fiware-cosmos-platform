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

import es.tid.cosmos.servicemanager.services.Service

/** Factory of ambari services.
  *
  * This can be improved by using reflection on this package.
  */
private[ambari] object AmbariServiceFactory {

  private val ambariServices: Set[AmbariServiceDetails] = Set(
    AmbariCosmosUserService,
    AmbariHCatalog,
    AmbariHdfs,
    AmbariHive,
    AmbariInfinityfsDriver,
    AmbariInfinityfsServer,
    AmbariMapReduce2,
    AmbariOozie,
    AmbariPig,
    AmbariSqoop,
    AmbariWebHCat,
    AmbariYarn,
    AmbariZookeeper
  )

  private val mapping: Map[Service, AmbariServiceDetails] = (for {
    ambariService <- ambariServices
  } yield ambariService.service -> ambariService).toMap

  def lookup(service: Service): AmbariServiceDetails =
    mapping.getOrElse(service, throw new IllegalArgumentException(
      s"Unsupported service $service: no mapping to Ambari services"
    ))
}
