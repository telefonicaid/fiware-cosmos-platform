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

package es.tid.cosmos.api.controllers.pages

import es.tid.cosmos.api.profile.{UserState, CosmosProfileDao, UserId, Registration}
import es.tid.cosmos.servicemanager.{ServiceManager, ClusterUser}

/** Sequence of actions to register a new user in Cosmos.
  *
  * @constructor
  * @param dao             For accessing to the store of Cosmos profiles
  * @param serviceManager  For registering the user credentials
  */
class UserRegistrationWizard(dao: CosmosProfileDao, serviceManager: ServiceManager) {

  /** Registers a new user.
    *
    * @param userId        User id
    * @param registration  Registration parameters
    * @return              Newly created profile
    */
  def registerUser(userId: UserId, registration: Registration) = dao.withTransaction { implicit c =>
    val newProfile = dao.registerUser(userId, registration)
    val clusterUsers = dao.getAllUsers().filter(_.state == UserState.Enabled).map(profile =>
      ClusterUser(
        userName = profile.handle,
        publicKey = profile.keys.head.signature,
        sshEnabled = false,
        hdfsEnabled = true
      ))
    serviceManager.setUsers(serviceManager.persistentHdfsId, clusterUsers)
    newProfile
  }
}
