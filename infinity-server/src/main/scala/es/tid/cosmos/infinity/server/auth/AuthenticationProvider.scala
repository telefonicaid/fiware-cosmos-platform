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

package es.tid.cosmos.infinity.server.auth

import scala.util.Try

/** An object able to provide functions to authenticate Infinity requests. */
trait AuthenticationProvider {

  /** Authenticate the given user from the given credentials
    *
    * @param credentials the credentials of the user to be authenticated
    * @return the user identity associated to the credentials, or an exception if the user
    *         cannot be authenticated.
    */
  def authenticate(credentials: UserCredentials): Try[UserProfile]

  /** Authenticate the given cluster from the given credentials
    *
    * @param credentials the credentials of the cluster to be authenticated
    * @return the user identity associated to the credentials, or an exception if the user
    *         cannot be authenticated.
    */
  def authenticate(credentials: ClusterCredentials): Try[UserProfile]
}
