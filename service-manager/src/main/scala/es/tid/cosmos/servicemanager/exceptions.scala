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

package es.tid.cosmos.servicemanager

import com.ning.http.client.Request

class ServiceException(message: String, cause: Throwable = null)
  extends RuntimeException(message, cause)

case class RequestException(request: Request, message: String, cause: Throwable = null)
  extends ServiceException(s"""$message
                               |Request: $request""".stripMargin, cause)

case class DeploymentException(href: String, cause: Throwable = null)
  extends ServiceException(
    "The cluster did not finish installing correctly. Please contact cosmos@tid.es and tell them" +
      s" that deployment failed at $href",
    cause)

case class ServiceError(message: String, cause: Throwable = null) extends Error(message, cause)