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

import es.tid.cosmos.api.task.{MutableTask, Running, TaskStatus}

class InMemoryTask(val id: Int) extends MutableTask {
  @volatile override var status: TaskStatus = Running
  @volatile override var usersWithAccess: Seq[String] = Seq.empty
  @volatile override var metadata: Any = ()
}
