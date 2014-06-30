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

package es.tid.cosmos.api.task.inmemory

import es.tid.cosmos.api.task.{ImmutableTask, MutableTask, TaskDao}
import scala.collection.concurrent.TrieMap

class InMemoryTaskDao extends TaskDao {
  private[inmemory] val tasks = TrieMap[Int, InMemoryTask]()

  override def registerTask(): MutableTask = this.synchronized {
    val task = new InMemoryTask(tasks.size)
    tasks += (task.id -> task)
    task
  }

  override def get(id: Int): Option[MutableTask] = tasks.get(id)

  override def list: Seq[ImmutableTask] = tasks.readOnlySnapshot().values.toSeq.view.map(_.view)
}
