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

package es.tid.cosmos.api.profile

/** Exception to be thrown by implementations of CosmosProfileDao to provide a clean
  * layer abstraction.
  *
  * @param message Error description
  * @param cause   Deeper cause of the problem
  */
case class CosmosProfileException(message: String, cause: Throwable = null)
  extends RuntimeException(message, cause)

object CosmosProfileException {

  def unknownUser(id: ProfileId) = CosmosProfileException(s"Unknown user with cosmosId=$id")

  def duplicatedHandle(handle: String) =
    CosmosProfileException(s"Cannot use $handle as handle or it will become duplicated")
}
