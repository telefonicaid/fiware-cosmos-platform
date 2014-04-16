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

import scala.concurrent.{Future, Await, future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._
import scala.language.postfixOps

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class SequentialOperationsTest extends FlatSpec with MustMatchers {

  var i = BigInt(1)

  "A sequential operation" must "run sequentially" in {
    val sequentialOperations = new SequentialOperations
    val numberOfOperations = 1000
    val results_> = Future.sequence((1 to numberOfOperations).map(a => {
      sequentialOperations enqueue {
        for {
          _ <- future { i = i + 1 }
          _ <- future { i = i * 2 }
        } yield i
      }
    }))
    val results = Await.result(results_>, 10 seconds)
    def expectedResultStream: Stream[BigInt] = BigInt(4) #:: expectedResultStream.map(a => 2 * (1 + a))
    results must be === expectedResultStream.take(numberOfOperations)
  }
}
