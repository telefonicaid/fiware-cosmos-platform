/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.common.util

import java.io.IOException
import java.util.concurrent.TimeoutException
import scala.concurrent.{Await, Future}
import scala.concurrent.duration.FiniteDuration
import scala.util.{Try, Failure, Success}
import scala.util.control.NonFatal

import org.apache.commons.logging.LogFactory

/** Time-bound waiting of future results */
class TimeBound(timeOut: FiniteDuration) {
  private val Log = LogFactory.getLog(classOf[TimeBound])

  /** Blocks for a result to be ready.
    *
    * @param result  Result to wait for
    * @return        The result if the future succeeds on time
    * @throws IOException If action fails or takes too much time
    */
  def awaitResult[T](result: Future[T]): T = boundedWait(result) match {
    case Success(value) => value
    case Failure(ex: IOException) => throw ex
    case Failure(ex) =>
      Log.error("Cannot perform Infinity file system action", ex)
      throw new IOException("Cannot perform Infinity action", ex)
  }

  /** Blocks for an action completion.
    *
    * @param action  Action to block for
    * @return        Whether the action succeeded
    */
  def awaitAction(action: => Future[_]): Boolean = {
    val task = try {
      action
    } catch {
      case NonFatal(ex) => Future.failed(ex)
    }
    boundedWait(task) match {
      case Success(_) => true
      case Failure(ex) =>
        Log.error("Cannot perform Infinity file system action", ex)
        false
    }
  }

  /** Blocks for a result to be ready (or failed) */
  private def boundedWait[T](result: Future[T]): Try[T] = try {
    Await.ready(result, timeOut).value.get
  } catch {
    case ex: TimeoutException => Failure(ex)
  }
}
