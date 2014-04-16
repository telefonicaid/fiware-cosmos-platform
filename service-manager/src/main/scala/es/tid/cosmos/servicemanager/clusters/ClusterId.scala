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

package es.tid.cosmos.servicemanager.clusters

import java.util.UUID

case class ClusterId(id: String) extends Ordered[ClusterId] {
  require(
    id.forall(Character.isLetterOrDigit),
    s"Cluster IDs can only contain alphanumeric characters. Provided id: %id")

  def this(uuid: UUID = UUID.randomUUID()) = this(uuid.toString.replace("-", ""))
  override def toString: String = id

  def compare(that: ClusterId): Int = this.id.compare(that.id)
}

object ClusterId {

  /** Creates a cluster ID using UUID.randomUUID. */
  def random() = new ClusterId(UUID.randomUUID())

  def apply(uuid: UUID) = new ClusterId(uuid)
}
