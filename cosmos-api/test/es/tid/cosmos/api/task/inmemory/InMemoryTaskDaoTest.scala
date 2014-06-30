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

import scala.concurrent.{Promise, Await, Future, future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration.Duration

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import es.tid.cosmos.api.task.{Failed, Finished, Running}
import org.scalatest.concurrent.{Eventually, IntegrationPatience}

class InMemoryTaskDaoTest extends FlatSpec with MustMatchers with Eventually with IntegrationPatience {
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

  it must "correctly link tasks to futures when the future succeeds" in {
    val dao = new InMemoryTaskDao
    val promise = Promise[Unit]()
    val task = dao.registerTask().linkToFuture(promise.future, "Foo")
    task.status must be (Running)
    promise.success()
    eventually { task.status must be (Finished) }
  }

  it must "correctly link tasks to futures when the future fails" in {
    val dao = new InMemoryTaskDao
    val promise = Promise[Unit]()
    val task = dao.registerTask().linkToFuture(promise.future, "Foo")
    task.status must be (Running)
    promise.failure(new IllegalArgumentException("Bar"))
    eventually { task.status must be (Failed("Foo")) }
  }
}
