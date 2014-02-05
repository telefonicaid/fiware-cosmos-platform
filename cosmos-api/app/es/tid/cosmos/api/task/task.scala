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

package es.tid.cosmos.api.task

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import play.Logger

sealed trait Task {
  val id: Int
  def status: TaskStatus
  def usersWithAccess: Seq[String]
  def resource: String
  def metadata: Any
  def view: ImmutableTask
}

trait MutableTask extends Task {
  var status: TaskStatus
  var usersWithAccess: Seq[String]
  var resource: String
  var metadata: Any
  def view: ImmutableTask = ImmutableTask(id, status, resource, usersWithAccess, metadata)
  final def linkToFuture(future: Future[Any], errorMessage: String): MutableTask = {
    future.onSuccess {
      case _ => status = Finished
    }
    future.onFailure {
      case err =>
        Logger.error(errorMessage, err)
        status = Failed(errorMessage)
    }
    this
  }
}

case class ImmutableTask(
    id: Int,
    status: TaskStatus,
    resource: String,
    usersWithAccess: Seq[String],
    metadata: Any) extends Task {
  val view = this
}
