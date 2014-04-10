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

package es.tid.cosmos.infinity.server.authentication

import java.net.InetAddress

import spray.http._
import spray.http.HttpHeaders.Authorization
import spray.http.HttpRequest
import spray.http.OAuth2BearerToken

/** The credentials used by a Infinity client to authenticate a request. */
sealed trait Credentials

object Credentials {

  /** Extract credentials from an HTTP request coming from a given remote address.
    *
    * @param remoteAddress  Address of the requester
    * @param request  HTTP request
    * @return User or cluster credentials
    * @throws CredentialsException if no valid credentials are found
    */
  def from(remoteAddress: InetAddress, request: HttpRequest): Credentials = {
    val Authorization(httpCredentials) =
      request.header[Authorization].getOrElse(throw MissingAuthorizationHeader)
    httpCredentials match {
      case BasicHttpCredentials(key, secret) => UserCredentials(key, secret)
      case OAuth2BearerToken(secret) => ClusterCredentials(remoteAddress.getHostName, secret)
      case header: GenericHttpCredentials => throw UnsupportedAuthorizationScheme(header)
    }
  }
}

/** The credentials used by users to authenticate their Infinity FS requests. */
case class UserCredentials(apiKey: String, apiSecret: String) extends Credentials

/** The credentials used by clusters to authenticate their Infinity FS requests. */
case class ClusterCredentials(hostname: String, clusterSecret: String) extends Credentials

sealed abstract class CredentialsException(message: String) extends Exception(message)
case object MissingAuthorizationHeader
  extends CredentialsException("Missing 'Authorization' header")
case class UnsupportedAuthorizationScheme(header: GenericHttpCredentials)
  extends CredentialsException(
    s"Unsupported authorization header with scheme ${header.scheme}; $header")
