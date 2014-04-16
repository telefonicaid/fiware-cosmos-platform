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
import scala.util.{Failure, Try, Success}
import scala.util.control.NonFatal
import scalaz.{Success => _, Failure => _, _}

import play.Logger
import play.api.mvc.Results

import es.tid.cosmos.api.controllers.common.ErrorMessage
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.dao.ProfileDataStore
import es.tid.cosmos.servicemanager.ServiceManager

/** Sequence of actions to register a new user in Cosmos.
  *
  * @constructor
  * @param store           Data store
  * @param serviceManager  For registering the user credentials
  */
class UserRegistrationWizard(store: ProfileDataStore, serviceManager: ServiceManager)
  extends Results {

  import Scalaz._

  private val hdfsWizard = new UpdatePersistentHdfsUsersWizard(store, serviceManager)

  /** Registers a new user.
    *
    * Database user creation is synchronously done but persistent HDFS account creation is done
    * as a background task.
    *
    * @param userId        User id
    * @param registration  Registration parameters
    * @return              Newly created profile or an error message
    */
  def registerUser(
        userId: UserId, registration: Registration
      ): Validation[ErrorMessage, (CosmosProfile, Future[Unit])] =
    store.withTransaction { implicit c =>
      Logger.info(s"Starting $userId (${registration.handle}) registration")
      Try (store.profile.register(userId, registration, UserState.Creating)) match {
        case Failure(ex) =>
          logRegistrationError(userId, ex)
          ErrorMessage(registrationErrorMessage(userId)).failure
        case Success(profile) =>
          val registration_> = for {
            _ <- hdfsWizard.updatePersistentHdfsUsers()
          } yield markUserEnabled(userId)
          registration_>.onComplete {
            case Failure(NonFatal(ex)) => logRegistrationError(userId, ex)
            case Success(_) => logRegistrationSuccess(userId, profile)
          }
          (profile, registration_>).success
      }
    }

  private def markUserEnabled(userId: UserId): Unit = store.withTransaction { implicit c =>
    store.profile.lookupByUserId(userId).map { profile =>
      if (profile.state == UserState.Creating) {
        store.profile.setUserState(profile.id, UserState.Enabled)
      } else {
        logRegistrationError(userId, new IllegalStateException(s"""
          | Registration for $userId (${profile.handle}) cannot be completed as it is in
          | '${profile.state}' state instead of 'creating' state." +
        """.stripMargin))
      }
    }.getOrElse {
      logRegistrationError(userId, new IllegalStateException(
        s"Cannot complete registration of unknown user $userId"))
    }
  }

  private def logRegistrationSuccess(userId: UserId, profile: CosmosProfile): Unit =
    Logger.info(s"User $userId (${profile.handle}) was successfully registered")

  private def logRegistrationError(userId: UserId, ex: Throwable): Unit =
    Logger.error(registrationErrorMessage(userId), ex)

  private def registrationErrorMessage(userId: UserId) = s"Registration of $userId failed"
}
