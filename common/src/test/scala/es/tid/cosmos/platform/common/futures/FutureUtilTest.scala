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

package es.tid.cosmos.platform.common.futures

import java.util.concurrent.atomic.AtomicInteger
import scala.concurrent.{future, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers

class FutureUtilTest extends FlatSpec with MustMatchers with FutureMatchers {

  val TestTimeout = 5 seconds

  "A task serialization" must "return a future empty seq when no task is passed" in {
    val result_> = FutureUtil.serializeTasks(Seq())
    result_> must runUnder(TestTimeout)
    result_> must eventually(be('empty))
  }

  it must "execute tasks in order" in {
    val lastExecuted = new AtomicInteger(0)
    val result_> = FutureUtil.serializeTasks(Seq(
      makeTask(1, 100 millis, lastExecuted),
      makeTask(2,   0 millis, lastExecuted),
      makeTask(3, 100 millis, lastExecuted),
      makeTask(4, 300 millis, lastExecuted)
    ))
    result_> must runUnder(TestTimeout)
    result_> must eventually(equal (Seq(1, 2, 3, 4)))
    lastExecuted.get must be (4)
  }

  private def makeTask(
      taskId: Int,
      delay: FiniteDuration,
      lastTaskExecuted: AtomicInteger): () => Future[Int] =
    () => future {
      Thread.sleep(delay.toMillis)
      lastTaskExecuted.getAndSet(taskId) must be (taskId - 1)
      taskId
    }
}
