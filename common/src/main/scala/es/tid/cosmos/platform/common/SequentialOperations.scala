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

package es.tid.cosmos.platform.common

import java.util.concurrent.Executors
import scala.concurrent._
import scala.concurrent.duration.Duration

class SequentialOperations {
  private implicit val executionContext: ExecutionContext = ExecutionContext.fromExecutor(
    ExecutionContext.fromExecutorService(Executors.newFixedThreadPool(1)))

  def enqueue[T](body: => Future[T]) = future { blocking {
    Await.result(body, Duration.Inf)
  }}
}
