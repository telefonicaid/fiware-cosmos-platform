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

package es.tid.cosmos.api.wizards

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global
import scala.util.control.NonFatal
import scalaz._

import play.Logger

import es.tid.cosmos.api.controllers.common.Message
import es.tid.cosmos.api.profile.{ClusterAssignment, UserState}
import es.tid.cosmos.api.profile.dao._
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{Terminating, Terminated, ClusterId}

class UserUnregistrationWizard(
    store: ProfileDataStore with ClusterDataStore,
    serviceManager: ServiceManager) {

  private val hdfsWizard = new UpdatePersistentHdfsUsersWizard(store, serviceManager)

  import Scalaz._

  type Unregistration = Future[Unit]

  case class DatabaseUnregistrationException(cosmosId: Long, cause: Throwable)
    extends RuntimeException(s"Cannot remove user with cosmosId=$cosmosId from the database", cause)

  case class ClusterTerminationException(cosmosId: Long, clusterId: ClusterId, reason: Throwable)
    extends RuntimeException(
      s"Cannot terminate cluster $clusterId of user with cosmosId=$cosmosId", reason)

  /** Creates an unregistration for a given user id when possible.
    *
    * The user is marked in deleting state within the passed transaction. The rest of the process
    * will be done in different DAO transactions.
    *
    * @param cosmosId  User to unregister
    * @return          The unregistration operation or a validation error
    */
  def unregisterUser(cosmosId: Long): Validation[Message, Unregistration] =
    for {
      _ <- markUserBeingDeleted(cosmosId)
    } yield startUnregistration(cosmosId)

  private def markUserBeingDeleted(cosmosId: Long): Validation[Message, Unit] = try {
    store.withTransaction { implicit c =>
      store.profile.setUserState(cosmosId, UserState.Deleting).success
    }
  } catch {
    case NonFatal(ex) =>
      val errorMessage = s"Cannot change user cosmosId=$cosmosId status"
      Logger.error(errorMessage, ex)
      Message(errorMessage).fail
  }

  private def startUnregistration(cosmosId: Long): Future[Unit] = {
    val clustersTermination_> = terminateClusters(cosmosId)
    val persistentHdfsCleanup_> = hdfsWizard.updatePersistentHdfsUsers()
    val userDisabledFromAllClusters_> = store.withTransaction { implicit c =>
      serviceManager.disableUserFromAll(store.profile.lookupByProfileId(cosmosId).get.handle)
    }
    for {
      _ <- clustersTermination_>
      _ <- persistentHdfsCleanup_>
      _ <- markUserDeleted(cosmosId)
      _ <- userDisabledFromAllClusters_>
    } yield ()
  }

  private def terminateClusters(cosmosId: Long) = Future.sequence(for {
    clusterId <- terminableClusters(cosmosId: Long)
    termination_> = serviceManager.terminateCluster(clusterId)
  } yield termination_>.transform(
    success => (),
    exception => ClusterTerminationException(cosmosId, clusterId, exception)
  ))

  private def terminableClusters(cosmosId: Long): Seq[ClusterId] = for {
    cluster <- profileClusters(cosmosId)
    description <- serviceManager.describeCluster(cluster.clusterId)
    if description.state != Terminated && description.state != Terminating
  } yield cluster.clusterId

  private def profileClusters(cosmosId: Long): Seq[ClusterAssignment] =
    store.withTransaction { implicit c =>
      store.cluster.ownedBy(cosmosId)
    }

  private def markUserDeleted(cosmosId: Long): Future[Unit] = Future {
    store.withTransaction { implicit c =>
      store.profile.setUserState(cosmosId, UserState.Deleted)
    }
  }.recoverWith {
    case NonFatal(ex) => Future.failed(DatabaseUnregistrationException(cosmosId, ex))
  }
}
