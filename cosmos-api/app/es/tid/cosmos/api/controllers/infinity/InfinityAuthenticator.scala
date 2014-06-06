/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.api.controllers.infinity

import scalaz._

import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.{ClusterDataStore, ProfileDataStore}
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{ClusterDescription, ClusterId, Running}

private[infinity] class InfinityAuthenticator(
    store: ProfileDataStore with ClusterDataStore,
    serviceManager: ServiceManager) {

  import Scalaz._

  def authenticateApiCredentials(credentials: ApiCredentials): Validation[Message, InfinityIdentity] =
    for {
      profile <- requireProfileWithCredentials(credentials)
      _ <- requireEnabledProfile(profile)
    } yield identifyAs(profile, shared = false)

  private def requireProfileWithCredentials(
      credentials: ApiCredentials): Validation[Message, CosmosProfile] =
    store.withTransaction { implicit c =>
        store.profile.lookupByApiCredentials(credentials)
    }.toSuccess(Message("Unknown API credentials"))

  private def requireEnabledProfile(profile: CosmosProfile): Validation[Message, Unit] =
    if (profile.state == UserState.Enabled) ().success
    else Message("User is not enabled").failure

  def authenticateClusterSecret(secret: ClusterSecret): Validation[Message, InfinityIdentity] =
    store.withTransaction { implicit c =>
      for {
        cluster <- store.cluster.lookupBySecret(secret).toSuccess(Message("Unknown cluster secret"))
        description <- requireRunningCluster(cluster.clusterId)
        profile <- store.profile.lookupByProfileId(cluster.ownerId)
          .toSuccess(Message("Orphan cluster: the user associated with it doesn't exist"))
        _ <- requireEnabledProfile(profile)
      } yield identifyAs(profile, cluster.shared, Some(clusterHosts(description)))
    }

  private def clusterHosts(cluster: ClusterDescription): Set[String] =
    cluster.hosts.map(_.ipAddress).toSet

  private def requireRunningCluster(clusterId: ClusterId): Validation[Message, ClusterDescription] =
    for {
      description <- serviceManager.describeCluster(clusterId)
        .toSuccess(Message("Cluster does not yet exist"))
      _ <- if (description.state != Running) Message("Cluster is not in running state").failure
           else ().success
    } yield description

  private def identifyAs(
      profile: CosmosProfile,
      shared: Boolean,
      whiteList: Option[Set[String]] = None) = InfinityIdentity(
    user = profile.handle,
    groups = Seq(profile.group.name),
    sharedCluster = shared,
    origins = whiteList
  )
}
