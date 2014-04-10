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

import spray.http.{HttpMethod, HttpMethods, HttpRequest}

import es.tid.cosmos.infinity.server.util.Path

trait ActionMapping {

  protected def opQueryParam(request: HttpRequest): String = queryParam(request, "op")

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
      request: HttpRequest,
      expectedMethod: HttpMethod)(eval: Path => T): T = {
    requireMethod(expectedMethod, request)
    val path = Path.absolute(request.uri.path.toString())
    eval(path)
  }

  protected def withGetBasicProps[T](request: HttpRequest)(eval: Path => T) =
    withBasicProps(request, HttpMethods.GET)(eval)

  protected def withPutBasicProps[T](request: HttpRequest)(eval: Path => T) =
    withBasicProps(request, HttpMethods.PUT)(eval)

  protected def withPostBasicProps[T](request: HttpRequest)(eval: Path => T) =
    withBasicProps(request, HttpMethods.POST)(eval)

  protected def withDeleteBasicProps[T](request: HttpRequest)(eval: Path => T) =
    withBasicProps(request, HttpMethods.DELETE)(eval)
}
