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
