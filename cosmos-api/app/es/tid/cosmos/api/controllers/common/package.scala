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

package es.tid.cosmos.api.controllers

import scala.concurrent.Future
import scala.language.implicitConversions
import scalaz.Validation

import play.api.mvc.SimpleResult

package object common {

  /** Represents intermediate play action computations that can fail.
    *
    * When they fail, action computation is short-circuited with a given
    * error response (of type SimpleResult).
    *
    * Tip: Star-import this package to get implicit conversions to `SimpleResult` and
    * `Future[SimpleResult]` and avoid manually folding the validation result on every action.
    *
    * @tparam T  Type of the value being computed. Unit for intermediate actions
    */
  type ActionValidation[T] = Validation[SimpleResult, T]

  /** Implicit conversion of asynchronous `ActionValidation` values to future simple results as
    * Play expects it.
    *
    * @param v Maybe successfully async computation
    * @return           A future of a simple response
    */
  implicit def toAsyncResult(v: ActionValidation[Future[SimpleResult]]): Future[SimpleResult] =
    v.fold(fail = Future.successful, succ = identity)

  /** Implicit conversion from `ActionValidation` to a simple result as play expects it.
    *
    * @param validation  Maybe successfully computed page
    * @return            A simple response
    */
  implicit def toSimpleResult(validation: ActionValidation[SimpleResult]): SimpleResult =
    validation.fold(fail = identity, succ = identity)
}
