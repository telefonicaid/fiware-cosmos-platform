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
    * @tparam T  Type of the value being computed. Unit for intermediate actions.
    */
  type ActionVal[T] = Validation[SimpleResult, T]

  /** Implicit conversion of asynchronous ActionVal values to future simple results as play
    * expects it.
    *
    * @param validation Maybe successfully async computation
    * @return           A future of a simple response
    */
  implicit def toAsyncResult(validation: ActionVal[Future[SimpleResult]]): Future[SimpleResult] =
    validation.fold(fail = Future.successful, succ = identity)

  /** Implicit conversion from ActionVal to a simple result as play expects it.
    *
    * @param validation  Maybe successfully computed page
    * @return            A simple response
    */
  implicit def toSimpleResult(validation: ActionVal[SimpleResult]): SimpleResult =
    validation.fold(fail = identity, succ = identity)

  implicit val ActionValErrorCombination: Monoid[SimpleResult] = new Monoid[SimpleResult] {
    def append(f1: SimpleResult, f2: => SimpleResult): SimpleResult = ???
    def zero: SimpleResult = ???
  }
}
