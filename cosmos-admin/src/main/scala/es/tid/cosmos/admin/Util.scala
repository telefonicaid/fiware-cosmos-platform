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

package es.tid.cosmos.admin

import scala.concurrent.{Await, Future}
import scala.concurrent.duration._
import scala.concurrent.ExecutionContext.Implicits._
import scala.language.postfixOps
import scala.util.{Failure, Success, Try}

private[admin] object Util {

  def waitUntilReady(task_> : Future[Unit]): Boolean = {
    val handled_> = task_>.transform(
      _ => println("Cosmos admin task succeded!"),
      throwable => {
        println("ERROR: Cosmos admin task failed")
        println(throwable.getMessage)
        println()
        println(throwable.getStackTrace.deep.mkString("\n"))
        throwable
      })
    Await.ready(handled_>, 15 minutes).value.get.isSuccess
  }

  def whenEmpty[T](value: Option[T])(action: => Unit): Option[T] = {
    if (value.isEmpty) action
    value
  }

  def tryAction(action: => Option[Any]): Boolean = {
    Try(action) match {
      case Success(maybe) => maybe.isDefined
      case Failure(e) => { println(s"Error: ${e.getMessage}"); false }
    }
  }
}
