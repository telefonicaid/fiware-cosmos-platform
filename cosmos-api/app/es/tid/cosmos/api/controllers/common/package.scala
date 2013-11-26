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
