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
            group = profile.group.hdfsGroupName,
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
