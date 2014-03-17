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

import es.tid.cosmos.servicemanager.ClusterUser

object CosmosUserService extends Service {
  override type Parametrization = Seq[ClusterUser]
  override val name: String = "COSMOS_USER"
  override def defaultParametrization: Option[Parametrization] = Some(Seq.empty)
  override val dependencies: Set[Service] = Set(Hdfs)
}
