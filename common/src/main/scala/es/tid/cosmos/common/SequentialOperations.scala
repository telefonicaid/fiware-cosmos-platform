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

package es.tid.cosmos.common

import java.util.concurrent.Executors
import scala.concurrent._
import scala.concurrent.duration.Duration

class SequentialOperations {
  private implicit val executionContext: ExecutionContext = ExecutionContext.fromExecutor(
    ExecutionContext.fromExecutorService(Executors.newFixedThreadPool(1)))

  /**
   * Futures passed to this function will be called sequentially. Each future will not start
   * until the previous one has finished completely
   * @param body The future that needs to be added to the execution queue
   * @return A future that finishes when body finishes
   */
  def enqueue[T](body: => Future[T]) = future { blocking {
    Await.result(body, Duration.Inf)
  }}
}
