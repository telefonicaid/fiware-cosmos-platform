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
import es.tid.cosmos.api.profile.{UserState, CosmosProfileDao}
import es.tid.cosmos.servicemanager.ServiceManager
import es.tid.cosmos.servicemanager.clusters.{Terminating, Terminated, ClusterId}

class UserUnregistrationWizard(serviceManager: ServiceManager, dao: CosmosProfileDao) {

  private val hdfsWizard = new UpdatePersistentHdfsUsersWizard(dao, serviceManager)

  import Scalaz._

  type Unregistration = Future[Unit]

  case class DatabaseUnregistrationException(cosmosId: Long, cause: Throwable)
    extends RuntimeException(s"Cannot remove user with cosmosId=$cosmosId from the database", cause)

  case class ClusterTerminationException(cosmosId: Long, clusterId: ClusterId, reason: Throwable)
    extends RuntimeException(
      s"Cannot terminate cluster $clusterId of user with cosmosId=$cosmosId", reason)

  /**
   * Creates an unregistration for a given user id when possible.
   *
   * @param cosmosId User to unregister
   * @return         The unregistration operation or a validation error
   */
  def unregisterUser(cosmosId: Long): Validation[Message, Unregistration] = for {
    _ <- markUserBeingDeleted(cosmosId)
  } yield startUnregistration(cosmosId)

  private def markUserBeingDeleted(cosmosId: Long): Validation[Message, Unit] =
    dao.withTransaction { implicit c =>
      try {
        dao.setUserState(cosmosId, UserState.Deleting).success
      } catch {
        case NonFatal(ex) => {
          val errorMessage = s"Cannot change user cosmosId=$cosmosId status"
          Logger.error(errorMessage, ex)
          Message(errorMessage).fail
        }
      }
    }

  private def startUnregistration(cosmosId: Long): Future[Unit] = {
    val clustersTermination_> = terminateClusters(cosmosId)
    val persistentHdfsCleanup_> = hdfsWizard.updatePersistentHdfsUsers()
    for {
      _ <- clustersTermination_>
      _ <- persistentHdfsCleanup_>
      _ <- markUserDeleted(cosmosId)
    } yield ()
  }

  private def terminateClusters(cosmosId: Long) = {
    val terminableClusters = for {
      cluster <- dao.withTransaction { implicit c =>
        dao.clustersOf(cosmosId)
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
  
  private def markUserDeleted(cosmosId: Long): Future[Unit] = Future {
    dao.withTransaction { implicit c =>
      dao.setUserState(cosmosId, UserState.Deleted)
    }
  }.recoverWith {
    case NonFatal(ex) => Future.failed(DatabaseUnregistrationException(cosmosId, ex))
  }
}
