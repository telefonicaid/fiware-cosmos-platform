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

package es.tid.cosmos.common

import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

/** Implicit method extensions that can be used to do short circuit logical operations
  * on Boolean futures only evaluating the right part of the operation if necessary.
  */
object BooleanFutures {

  implicit class BooleanFutureOps(val left: Future[Boolean]) extends AnyVal {
    /** An OR, logical inclusive disjunction operation.
      * Maps this future to the right part only if this future's value evaluates to false.
      * @param right the right boolean future of the OR
      * @return true if at least one of the futures is true
      */
    def or(right: => Future[Boolean]): Future[Boolean] =
      left.flatMap(value => if (value) Future.successful(value) else right)

    /** An AND, logical conjunction operation.
      * Maps this future to the right part only if this future's value evaluates to true.
      * @param right the right boolean future of the AND
      * @return true if at both one of the futures are true
      */
    def and(right: => Future[Boolean]): Future[Boolean] =
      left.flatMap(value => if (!value) Future.successful(value) else right)
  }
}
