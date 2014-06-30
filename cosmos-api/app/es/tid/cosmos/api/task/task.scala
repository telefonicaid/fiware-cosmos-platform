/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
