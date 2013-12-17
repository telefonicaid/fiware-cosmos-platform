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
import scala.util.{Failure, Try, Success}
import scala.util.control.NonFatal
import scalaz.{Success => _, Failure => _, _}

import play.Logger
import play.api.libs.json.Json
import play.api.mvc.Results

import es.tid.cosmos.api.controllers.common.{ErrorMessage, ActionValidation}
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
    * @return              Newly created profile or an error response
    */
  def registerUser(
      dao: CosmosProfileDao,
      userId: UserId,
      registration: Registration): ActionValidation[CosmosProfile] =
    dao.withTransaction { implicit c =>
      Try (dao.registerUser(userId, registration)) match {
        case Failure(ex) => {
          logRegistrationError(userId, ex)
          InternalServerError(Json.toJson(ErrorMessage(registrationErrorMessage(userId)))).failure
        }
        case Success(profile) => {
          hdfsWizard.updatePersistentHdfsUsers(dao).onFailure {
            case NonFatal(ex) => logRegistrationError(userId, ex)
          }
          profile.success
        }
      }
    }

  private def logRegistrationError(userId: UserId, ex: Throwable) {
    Logger.error(registrationErrorMessage(userId), ex)
  }

  private def registrationErrorMessage(userId: UserId) = s"Registration of $userId failed"
}
