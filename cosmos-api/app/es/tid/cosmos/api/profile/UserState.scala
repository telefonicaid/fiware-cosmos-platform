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

package es.tid.cosmos.api.profile

/**
 * States of the cosmos user lifecycle.
 */
object UserState extends Enumeration {

  type UserState = Value

  /**
   * User is active and can operate within his/her quota.
   */
  val Enabled = Value("enabled")

  /**
   * User exists but cannot perform operations.
   */
  val Disabled = Value("disabled")

  /**
   * User is in the process of being deleted.
   */
  val Deleting = Value("deleting")

  /**
   * User has been already deleted.
   */
  val Deleted = Value("deleted")

  /**
   * Extractor of user states.
   *
   * @see <a href="http://www.scala-lang.org/old/node/112">Extractor Objects</a>
   */
  def unapply(state: String): Option[UserState] = values.find(_.toString == state.toLowerCase)
}
