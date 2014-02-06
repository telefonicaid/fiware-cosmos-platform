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
