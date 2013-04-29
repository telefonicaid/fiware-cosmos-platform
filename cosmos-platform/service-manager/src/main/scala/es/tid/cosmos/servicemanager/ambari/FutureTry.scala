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
package es.tid.cosmos.servicemanager.ambari

import scala.util.{Failure, Success, Try}
import scala.concurrent.Future

object FutureTry {
  def apply[T](r: =>T): Future[T] = Try(r) match {
    case Success(ret) => Future.successful(ret)
    case Failure(ex) => Future.failed(ex)
  }
}
