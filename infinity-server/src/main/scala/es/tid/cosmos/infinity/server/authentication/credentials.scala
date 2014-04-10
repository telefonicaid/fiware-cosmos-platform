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

import spray.http.{BasicHttpCredentials, HttpHeaders, HttpRequest, OAuth2BearerToken}

/** The credentials used by a Infinity client to authenticate a request. */
sealed trait Credentials

object Credentials {

  def from(remoteAddress: InetAddress, request: HttpRequest): Option[Credentials] = for {
    HttpHeaders.Authorization(httpCredentials) <- request.header[HttpHeaders.Authorization]
    credentials <- httpCredentials match {
      case BasicHttpCredentials(key, secret) => Some(UserCredentials(key, secret))
      case OAuth2BearerToken(secret) => Some(ClusterCredentials(remoteAddress.getHostName, secret))
      case _ => None
    }
  } yield credentials
}

/** The credentials used by users to authenticate their Infinity FS requests. */
case class UserCredentials(apiKey: String, apiSecret: String) extends Credentials

/** The credentials used by clusters to authenticate their Infinity FS requests. */
case class ClusterCredentials(hostname: String, clusterSecret: String) extends Credentials
