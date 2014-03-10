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

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future
import scala.util.control.NonFatal

import play.Logger

import es.tid.cosmos.api.profile.UserState._
import es.tid.cosmos.api.profile.dao.ProfileDataStore
import es.tid.cosmos.api.wizards.UpdatePersistentHdfsUsersWizard._
import es.tid.cosmos.common.SequentialOperations
import es.tid.cosmos.servicemanager.{ClusterUser, ServiceManager}

/** Sequence of actions to update the users configured in the persistent HDFS cluster.
  *
  * @constructor
  * @param store           Data store to manipulate user profiles
  * @param serviceManager  To act on the cluster
  */
class UpdatePersistentHdfsUsersWizard(store: ProfileDataStore, serviceManager: ServiceManager) {

  case class PersistentHdfsUpdateException(cause: Throwable)
    extends RuntimeException("Cannot update persistent HDFS users", cause)

  def updatePersistentHdfsUsers(): Future[Unit] =
    persistentHdfsOperationsQueue.enqueue {
      val clusterUsers = store.withTransaction { implicit c =>
        store.profile.list().map { profile =>
          ClusterUser(
            username = profile.handle,
            publicKey = profile.keys.head.signature,
            sshEnabled = false,
            hdfsEnabled = AllowedUserStates.contains(profile.state)
          )
        }
      }
      serviceManager.setUsers(serviceManager.persistentHdfsId, clusterUsers).recoverWith {
        case NonFatal(ex) =>
          val updateException = PersistentHdfsUpdateException(ex)
          Logger.error(updateException.getMessage, ex)
          Future.failed(updateException)
      }
  }
}

object UpdatePersistentHdfsUsersWizard {
  /** Profiles in these user states should get HDFS access configured. */
  private val AllowedUserStates: Set[UserState] = Set(Creating, Enabled)
  private val persistentHdfsOperationsQueue = new SequentialOperations()
}
