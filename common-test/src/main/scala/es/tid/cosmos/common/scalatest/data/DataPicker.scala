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

package es.tid.cosmos.common.scalatest.data

import scala.util.Random

/**
  * Utility trait for picking data out of a set.
  */
trait DataPicker {

  /** fixed seed for replicating random generation sequence */
  protected val seed: Int

  val random = new Random(seed)

  /**
    * Randomly pick an element from the given iterable container.
    *
    * @param from the container of elements to pick from
    * @tparam T   the type of elements to pick from
    * @return     the randomly picked element
    */
  def pickAnyOne[T](from: Iterable[T]): T = {
    def randomBelow(limit: Int): Int = random.nextInt(limit)
    from.toList(randomBelow(from.size))
  }
}
