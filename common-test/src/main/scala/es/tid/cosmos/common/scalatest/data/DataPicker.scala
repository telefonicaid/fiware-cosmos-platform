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
