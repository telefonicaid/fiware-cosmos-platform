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

import scala.concurrent.{Await, Future, future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration.Duration

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class InMemoryTaskDaoTest extends FlatSpec with MustMatchers {
  "The InMemoryTaskDao" must "return incremental IDs every time a new task is registered" in {
    val dao = new InMemoryTaskDao
    for (i <- 0 to 20) {
      dao.registerTask().id must be (i)
    }
  }

  it must "return be able to list all the tasks" in {
    val dao = new InMemoryTaskDao
    for (i <- 0 to 20) {
      dao.registerTask()
    }
    dao.list.map(_.id).sorted must be (0 to 20)
  }

  it must "map each task to its ID" in {
    val dao = new InMemoryTaskDao
    val task = dao.registerTask()
    dao.get(task.id) must be (Some(task))
  }

  it must "return None if the ID does not correspond to a task" in {
    val dao = new InMemoryTaskDao
    dao.get(100) must be (None)
  }

  it must "not return duplicate ID in a concurrent scenario" in {
    val petitionCount = 100
    val dao = new InMemoryTaskDao
    val concurrentRegistrations = Future.sequence(Seq.fill(petitionCount)(future { dao.registerTask() }))
    val tasks = Await.result(concurrentRegistrations, Duration.Inf)
    tasks.map(_.id).sorted must be (0 to (petitionCount - 1))
  }
}
