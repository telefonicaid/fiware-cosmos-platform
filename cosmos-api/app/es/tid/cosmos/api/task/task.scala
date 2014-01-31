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

sealed trait Task {
  val id: Int
  def status: TaskStatus
  def usersWithAccess: Seq[String]
  def metadata: Any
  def view: ImmutableTask
}

trait MutableTask extends Task {
  var status: TaskStatus
  var usersWithAccess: Seq[String]
  var metadata: Any
  def view: ImmutableTask = ImmutableTask(id, status, usersWithAccess, metadata)
}

case class ImmutableTask(
    id: Int,
    status: TaskStatus,
    usersWithAccess: Seq[String],
    metadata: Any) extends Task {
  val view = this
}