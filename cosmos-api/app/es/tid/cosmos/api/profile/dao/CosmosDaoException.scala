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

package es.tid.cosmos.api.profile.dao

import es.tid.cosmos.api.profile.ProfileId

/** Exception to be thrown by implementations of DAOs to provide a clean
  * layer abstraction.
  *
  * @param message Error description
  * @param cause   Deeper cause of the problem
  */
case class CosmosDaoException(message: String, cause: Throwable = null)
  extends RuntimeException(message, cause)

object CosmosDaoException {

  def unknownUser(id: ProfileId) = CosmosDaoException(s"Unknown user with cosmosId=$id")

  def duplicatedHandle(handle: String) =
    CosmosDaoException(s"Cannot use $handle as handle or it will become duplicated")
}
