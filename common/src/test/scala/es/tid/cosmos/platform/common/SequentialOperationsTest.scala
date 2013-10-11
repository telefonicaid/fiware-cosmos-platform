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

package es.tid.cosmos.platform.common

import scala.concurrent.{Future, Await, future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration._

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
