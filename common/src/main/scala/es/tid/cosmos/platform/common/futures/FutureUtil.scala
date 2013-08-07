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

import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

object FutureUtil {

  /**
   * Create a future object representing the serial computation of the tasks passed as argument.
   * The resulting futures will represent the sequential execution of the futures returned by
   * the tasks.
   *
   * @param tasks A sequence of functions that produces the futures to be serialized
   * @tparam T The result type of the future
   * @return A future representing the sequential execution of the tasks
   */
  def serializeTasks[T](tasks: Seq[() => Future[T]]) : Future[Seq[T]] = {
    def step(accum_> : Future[Seq[T]], task: () => Future[T]) = for {
      results <- accum_>
      taskResult <- task()
    } yield results ++ Seq(taskResult)
    val initialAccum_> = Future.successful(Seq[T]())
    tasks.foldLeft(initialAccum_>)(step)
  }
}
