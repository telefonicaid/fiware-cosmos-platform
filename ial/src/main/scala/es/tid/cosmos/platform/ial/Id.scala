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
