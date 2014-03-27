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

package es.tid.cosmos.infinity.server.actions

import java.net.InetAddress

import spray.http.{HttpMethods, HttpMethod, HttpRequest}

import es.tid.cosmos.infinity.server.authentication.{ClusterCredentials, UserCredentials, Credentials}
import es.tid.cosmos.infinity.server.util.Path

trait ActionMapping {

  protected def opQueryParam(request: HttpRequest): String = queryParam(request, "op")

  protected def credentialsFrom(remoteAddress: InetAddress, request: HttpRequest): Credentials = {
    val apiKey = queryParamOpt(request, "api.key")
    val apiSecret = queryParamOpt(request, "api.secret")
    val clusterSecret = queryParamOpt(request, "cluster.secret")
    (apiKey, apiSecret, clusterSecret) match {
      case (Some(key), Some(secret), None) => UserCredentials(key, secret)
      case (None, None, Some(secret)) => ClusterCredentials(remoteAddress.getHostName, secret)
      case _ => throw new IllegalArgumentException(
        s"invalid authentication query params in ${request.uri.toString()}")
    }
  }

  protected def queryParamOpt(request: HttpRequest, paramName: String): Option[String] =
    request.uri.query.get(paramName)

  protected def queryParam(request: HttpRequest, paramName: String): String =
    queryParamOpt(request, paramName).getOrElse(
      throw new IllegalArgumentException(s"missing '$paramName' query param in URI ${request.uri}"))

  protected def requireMethod(method: HttpMethod, request: HttpRequest): Unit =
    require(request.method == method,
      s"invalid HTTP method ${request.method} while expecting $method in $request")

  protected def requireGetMethod(request: HttpRequest): Unit = requireMethod(HttpMethods.GET, request)
  protected def requirePutMethod(request: HttpRequest): Unit = requireMethod(HttpMethods.PUT, request)
  protected def requirePostMethod(request: HttpRequest): Unit = requireMethod(HttpMethods.POST, request)
  protected def requireDeleteMethod(request: HttpRequest): Unit = requireMethod(HttpMethods.DELETE, request)

  protected def withBasicProps[T](
      remoteAddress: InetAddress,
      request: HttpRequest,
      expectedMethod: HttpMethod)(eval: (Path, Credentials) => T): T = {
    requireMethod(expectedMethod, request)
    val path = Path.absolute(request.uri.path.toString())
    val credentials = credentialsFrom(remoteAddress, request)
    eval(path, credentials)
  }

  protected def withGetBasicProps[T](
      remoteAddress: InetAddress, request: HttpRequest)(eval: (Path, Credentials) => T) =
    withBasicProps(remoteAddress, request, HttpMethods.GET)(eval)

  protected def withPutBasicProps[T](
      remoteAddress: InetAddress, request: HttpRequest)(eval: (Path, Credentials) => T) =
    withBasicProps(remoteAddress, request, HttpMethods.PUT)(eval)

  protected def withPostBasicProps[T](
      remoteAddress: InetAddress, request: HttpRequest)(eval: (Path, Credentials) => T) =
    withBasicProps(remoteAddress, request, HttpMethods.POST)(eval)

  protected def withDeleteBasicProps[T](
      remoteAddress: InetAddress, request: HttpRequest)(eval: (Path, Credentials) => T) =
    withBasicProps(remoteAddress, request, HttpMethods.DELETE)(eval)

}
