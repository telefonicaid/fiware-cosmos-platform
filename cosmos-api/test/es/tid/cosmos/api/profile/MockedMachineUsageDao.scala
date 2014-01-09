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

package es.tid.cosmos.api.profile

import es.tid.cosmos.servicemanager.clusters.{ClusterId, ClusterDescription}

case class MockedMachineUsageDao(
      override val machinePoolSize: Int,
      groups: Set[Group],
      groupsToProfiles: Map[Group, Set[CosmosProfile]],
      profilesToClusters: Map[CosmosProfile, List[ClusterDescription]]) extends MachineUsageDao {

  override def usedMachinesByGroups(requestedClusterId: Option[ClusterId]): Map[Group, Int] = (for {
    group <- groups.toSeq
    profiles = groupsToProfiles(group).toSeq
    groupUsedMachinesSize = profiles.map(usedMachinesForActiveClusters(_, requestedClusterId)).sum
  } yield group -> groupUsedMachinesSize).toMap

  override def usedMachinesForActiveClusters(
      profile: CosmosProfile, requestedClusterId: Option[ClusterId]): Int = (for {
    description <- profilesToClusters(profile) if Some(description.id) != requestedClusterId
    clusterSize = description.expectedSize
  } yield clusterSize).sum
}
