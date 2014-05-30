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
import scala.concurrent.{Future, Promise}
import scala.concurrent.duration._

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class TimeBoundTest extends FlatSpec with MustMatchers {

  "A time bound result" must "succeed when it becomes available within time limits" in {
    val result = timeBound.awaitResult(Future.successful("Success"))
    result must be ("Success")
  }

  it must "fail with IOException when future does not complete within time limits" in {
    evaluating { timeBound.awaitResult(Promise().future) } must produce [IOException]
  }

  it must "fail with IOException when future fails within time limits" in {
    evaluating {
      timeBound.awaitResult(Future.failed(new RuntimeException("oops")))
    } must produce [IOException]
  }

  "A time bound action" must "be true when action completes within time limits" in {
    timeBound.awaitAction(Future.successful()) must be (true)
  }

  it must "be false when action does not complete within time limits" in {
    timeBound.awaitAction(Promise().future) must be (false)
  }

  it must "be false when action future fails within time limits" in {
    timeBound.awaitAction(Future.failed(new RuntimeException("oops"))) must be (false)
  }

  it must "be false when action evaluation fails" in {
    timeBound.awaitAction(sys.error("oops")) must be (false)
  }

  val timeBound = new TimeBound(1.second)
}
