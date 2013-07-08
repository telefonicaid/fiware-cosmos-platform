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

private[admin] object Util {

  def waitUntilReady(task_> : Future[Unit]) = {
    task_>.onSuccess({
      case _ => println("Cosmos admin task succeded!")
    })
    task_>.onFailure({
      case throwable => {
        println("ERROR: Cosmos admin task failed")
        println(throwable.getMessage)
        println()
        println(throwable.getStackTrace.deep.mkString("\n"))
      }})
    Await.ready(task_>, 15 minutes).value.get.isSuccess
  }
}
