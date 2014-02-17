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
import scala.util.{Failure, Try, Success}
import scala.util.control.NonFatal
import scalaz.{Success => _, Failure => _, _}

import play.Logger
import play.api.mvc.Results

import es.tid.cosmos.api.controllers.common.ErrorMessage
import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Registration
import es.tid.cosmos.servicemanager.ServiceManager

/** Sequence of actions to register a new user in Cosmos.
  *
  * @constructor
  * @param serviceManager  For registering the user credentials
  */
class UserRegistrationWizard(serviceManager: ServiceManager) extends Results {

  import Scalaz._

  private val hdfsWizard = new UpdatePersistentHdfsUsersWizard(serviceManager)

  /** Registers a new user.
    *
    * Database user creation is synchronously done but persistent HDFS account creation is done
    * as a background task.
    *
    * @param dao           For accessing to the store of Cosmos profiles
    * @param userId        User id
    * @param registration  Registration parameters
    * @return              Newly created profile or an error message
    */
  def registerUser(
      dao: CosmosProfileDao,
      userId: UserId,
      registration: Registration): Validation[ErrorMessage, (CosmosProfile, Future[Unit])] =
    dao.withTransaction { implicit c =>
      Logger.info(s"Starting $userId (${registration.handle}) registration")
      Try (dao.registerUser(userId, registration, UserState.Creating)) match {
        case Failure(ex) => {
          logRegistrationError(userId, ex)
          ErrorMessage(registrationErrorMessage(userId)).failure
        }
        case Success(profile) => {
          val registration_> = for {
            _ <- hdfsWizard.updatePersistentHdfsUsers(dao)
          } yield markUserEnabled(dao, userId)
          registration_>.onComplete {
            case Failure(NonFatal(ex)) => logRegistrationError(userId, ex)
            case Success(_) => logRegistrationSuccess(userId, profile)
          }
          (profile, registration_>).success
        }
      }
    }

  private def markUserEnabled(dao: CosmosProfileDao, userId: UserId) {
    dao.withTransaction { implicit c =>
      dao.lookupByUserId(userId).map { profile =>
        if (profile.state == UserState.Creating) {
          dao.setUserState(profile.id, UserState.Enabled)
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
  }

  private def logRegistrationSuccess(userId: UserId, profile: CosmosProfile) {
    Logger.info(s"User $userId (${profile.handle}) was successfully registered")
  }

  private def logRegistrationError(userId: UserId, ex: Throwable) {
    Logger.error(registrationErrorMessage(userId), ex)
  }

  private def registrationErrorMessage(userId: UserId) = s"Registration of $userId failed"
}
