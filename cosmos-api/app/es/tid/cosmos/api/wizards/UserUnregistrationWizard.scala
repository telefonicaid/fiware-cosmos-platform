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
import es.tid.cosmos.api.profile.UserState
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{Terminating, Terminated, ClusterId}
import es.tid.cosmos.api.profile.dao.CosmosDao

class UserUnregistrationWizard(serviceManager: ServiceManager) {

  private val hdfsWizard = new UpdatePersistentHdfsUsersWizard(serviceManager)

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
    * @param dao       To mark the user in deleting state and finally, deleted
    * @param cosmosId  User to unregister
    * @return          The unregistration operation or a validation error
    */
  def unregisterUser(dao: CosmosDao, cosmosId: Long): Validation[Message, Unregistration] =
    for {
      _ <- markUserBeingDeleted(dao, cosmosId)
    } yield startUnregistration(dao, cosmosId)

  private def markUserBeingDeleted(
      dao: CosmosDao, cosmosId: Long): Validation[Message, Unit] = try {
    dao.store.withTransaction { implicit c =>
      dao.profile.setUserState(cosmosId, UserState.Deleting).success
    }
  } catch {
    case NonFatal(ex) => {
      val errorMessage = s"Cannot change user cosmosId=$cosmosId status"
      Logger.error(errorMessage, ex)
      Message(errorMessage).fail
    }
  }

  private def startUnregistration(dao: CosmosDao, cosmosId: Long): Future[Unit] = {
    val clustersTermination_> = terminateClusters(dao, cosmosId)
    val persistentHdfsCleanup_> = dao.store.withTransaction { implicit c =>
      hdfsWizard.updatePersistentHdfsUsers(dao)
    }
    val userDisabledFromAllClusters_> = dao.store.withTransaction { implicit c =>
      serviceManager.disableUserFromAll(dao.profile.lookupByProfileId(cosmosId).get.handle)
    }
    for {
      _ <- clustersTermination_>
      _ <- persistentHdfsCleanup_>
      _ <- markUserDeleted(dao, cosmosId)
      _ <- userDisabledFromAllClusters_>
    } yield ()
  }

  private def terminateClusters(dao: CosmosDao, cosmosId: Long) = {
    val terminableClusters = for {
      cluster <- dao.store.withTransaction { implicit c =>
        dao.cluster.ownedBy(cosmosId)
      }
      description <- serviceManager.describeCluster(cluster.clusterId)
      if description.state != Terminated && description.state != Terminating
    } yield cluster.clusterId

    Future.sequence(for {
      clusterId <- terminableClusters
      termination_> = serviceManager.terminateCluster(clusterId)
    } yield termination_>.transform(
      success => (),
      exception => ClusterTerminationException(cosmosId, clusterId, exception)
    ))
  }

  private def markUserDeleted(dao: CosmosDao, cosmosId: Long): Future[Unit] = Future {
    dao.store.withTransaction { implicit c =>
      dao.profile.setUserState(cosmosId, UserState.Deleted)
    }
  }.recoverWith {
    case NonFatal(ex) => Future.failed(DatabaseUnregistrationException(cosmosId, ex))
  }
}
