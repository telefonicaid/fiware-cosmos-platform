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

import es.tid.cosmos.api.profile.{UserState, CosmosProfileDao}
import es.tid.cosmos.servicemanager.{ClusterUser, ServiceManager}

/** Sequence of actions to update the users configured in the persistent HDFS cluster.
  *
  * @constructor
  * @param dao             To get the user profiles
  * @param serviceManager  To act on the cluster
  */
class UpdatePersistentHdfsUsersWizard(dao: CosmosProfileDao, serviceManager: ServiceManager) {

  case class PersistentHdfsUpdateException(cause: Throwable)
    extends RuntimeException("Cannot update persistent HDFS users", cause)

  def updatePersistentHdfsUsers(): Future[Unit] = {
    val clusterUsers = dao.withTransaction { implicit c =>
      dao.getAllUsers().map(profile =>
        ClusterUser(
          userName = profile.handle,
          publicKey = profile.keys.head.signature,
          sshEnabled = false,
          hdfsEnabled = profile.state == UserState.Enabled
        ))
    }
    serviceManager.setUsers(serviceManager.persistentHdfsId, clusterUsers).recoverWith {
      case NonFatal(ex) => Future.failed(PersistentHdfsUpdateException(ex))
    }
  }
}
