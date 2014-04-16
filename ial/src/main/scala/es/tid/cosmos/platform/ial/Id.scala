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

package es.tid.cosmos.platform.ial

import java.util.UUID

/**
 * Resource identifier class.
 *
 * @param id the string representation of the identifier
 * @tparam T the type of resource which is being identified
 */
case class Id[+T](id: String) {
  override def toString = id
}

/**
 * Id class companion object.
 */
object Id {
  /**
   * Obtain a new Id object from a random UUID.
   *
   * @tparam T the type of resource the resulting Id is identifying
   * @return the newly created Id
   */
  def apply[T]: Id[T] =  new Id[T](UUID.randomUUID().toString)

  /**
   * Obtain a new Id object from given UUID.
   *
   * @param uuid the UUID used to populate the newly created Id
   * @tparam T the type of resource the resulting Id is identifying
   * @return the newly created Id
   */
  def apply[T](uuid: UUID): Id[T] = new Id[T](uuid.toString)
}
