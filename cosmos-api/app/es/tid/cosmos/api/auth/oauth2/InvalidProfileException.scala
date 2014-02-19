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

package es.tid.cosmos.api.auth.oauth2

/** A profile retrieved from an authentication provider cannot be parsed */
case class InvalidProfileException(profile: String, errors: Seq[String], cause: Throwable = null)
  extends IllegalArgumentException(
    s"Cannot parse user profile: ${errors.mkString(", ")}\nprofile = $profile", cause)
