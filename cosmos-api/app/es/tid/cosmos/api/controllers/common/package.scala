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
import scalaz.{Monoid, Validation}

import play.api.mvc.SimpleResult

package object common {

  /** Represents intermediate play action computations that can fail.
    *
    * When they fail, action computation is short-circuited with a given
    * error response (of type SimpleResult).
    *
    * @tparam T  Type of the value being computed. Unit for intermediate actions
    */
  type ActionValidation[T] = Validation[SimpleResult, T]

  /** Implicit conversion of asynchronous ActionVal values to future simple results as play
    * expects it.
    *
    * @param v Maybe successfully async computation
    * @return           A future of a simple response
    */
  implicit def toAsyncResult(v: ActionValidation[Future[SimpleResult]]): Future[SimpleResult] =
    v.fold(fail = Future.successful, succ = identity)

  /** Implicit conversion from ActionVal to a simple result as play expects it.
    *
    * @param validation  Maybe successfully computed page
    * @return            A simple response
    */
  implicit def toSimpleResult(validation: ActionValidation[SimpleResult]): SimpleResult =
    validation.fold(fail = identity, succ = identity)

  implicit val ActionValErrorCombination: Monoid[SimpleResult] = new Monoid[SimpleResult] {

    /** Stop at the first error */
    def append(f1: SimpleResult, f2: => SimpleResult): SimpleResult = f1

    /** Fail if we filter out  */
    def zero: SimpleResult = throw new IllegalStateException(
      "Filter was used on a validation without any means to generate an error response")
  }
}
