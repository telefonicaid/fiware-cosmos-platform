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

package es.tid.cosmos.api.controllers.common

abstract class AuthError(message: String) extends Throwable(message)
case class MalformedAuthHeader(header: String) extends AuthError(
  s"Malformed authorization header: expected base64 encoded API key and secret but got $header")
case object MissingAuthHeader extends AuthError("Missing authorization header")
case object InvalidAuthCredentials extends AuthError("Invalid API credentials")
