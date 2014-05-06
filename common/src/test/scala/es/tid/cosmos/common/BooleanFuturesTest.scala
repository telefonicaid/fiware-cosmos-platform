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

import scala.concurrent._
import scala.concurrent.Future.successful
import scala.concurrent.ExecutionContext.Implicits.global

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.BooleanFutures._
import es.tid.cosmos.common.scalatest.matchers.FutureMatchers

class BooleanFuturesTest extends FlatSpec with MustMatchers with FutureMatchers {

  "A boolean-futures OR" must "be true if at least one is true" in {
    successful(true) or successful(false) must eventually (be (true))
    successful(false) or successful(true) must eventually (be (true))
    successful(true) or successful(true) must eventually (be (true))
  }

  it must "be false if both are false" in {
    successful(false) or successful(false) must eventually (be (false))
  }

  it must "only evaluate left if left becomes true" in new EvaluationTracking {
    future { isLeftEvaluated = true; true } or future { isRightEvaluated = true; true } must
      eventually (be (true))
    isLeftEvaluated must be (true)
    isRightEvaluated must be (false)
  }

  it must "evaluate right if left becomes false" in new EvaluationTracking {
    future { isLeftEvaluated = true; false } or future { isRightEvaluated = true; true } must
      eventually (be (true))
    isLeftEvaluated must be (true)
    isRightEvaluated must be (true)
  }

  "A boolean-futures AND" must "be true if at both are true" in {
    successful(true) and successful(true) must eventually (be (true))
   }

  it must "be false if at least one is false" in {
    successful(false) and successful(true) must eventually (be (false))
    successful(true) and successful(false) must eventually (be (false))
    successful(false) and successful(false) must eventually (be (false))
  }

  it must "only evaluate left if left becomes false" in new EvaluationTracking {
    future { isLeftEvaluated = true; false } and future { isRightEvaluated = true; true } must
      eventually (be (false))
    isLeftEvaluated must be (true)
    isRightEvaluated must be (false)
  }

  it must "evaluate right if left becomes true" in new EvaluationTracking {
    future { isLeftEvaluated = true; true } and future { isRightEvaluated = true; false } must
      eventually (be (false))
    isLeftEvaluated must be (true)
    isRightEvaluated must be (true)
  }

  trait EvaluationTracking {
    @volatile var isLeftEvaluated = false
    @volatile var isRightEvaluated = false
  }
}
