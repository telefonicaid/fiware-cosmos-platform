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

import es.tid.cosmos.api.profile._
import es.tid.cosmos.api.profile.Registration
import es.tid.cosmos.servicemanager.ServiceManager

/** Sequence of actions to register a new user in Cosmos.
  *
  * @constructor
  * @param serviceManager  For registering the user credentials
  */
class UserRegistrationWizard(serviceManager: ServiceManager) {

  private val hdfsWizard = new UpdatePersistentHdfsUsersWizard(serviceManager)

  /** Registers a new user.
    *
    * @param dao             For accessing to the store of Cosmos profiles
    * @param userId        User id
    * @param registration  Registration parameters
    * @return              Newly created profile
    */
  def registerUser(dao: CosmosProfileDao, userId: UserId, registration: Registration)
                  (implicit c: dao.type#Conn): CosmosProfile = {
    val newProfile = dao.registerUser(userId, registration)(c)
    hdfsWizard.updatePersistentHdfsUsers(dao)
    newProfile
  }
}
