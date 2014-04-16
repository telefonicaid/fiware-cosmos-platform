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

package es.tid.cosmos.api.profile

/** States of the cosmos user lifecycle. */
object UserState extends Enumeration {

  type UserState = Value

  /** User is being created. */
  val Creating = Value("creating")

  /** User is active and can operate within his/her quota. */
  val Enabled = Value("enabled")

  /** User exists but cannot perform operations. */
  val Disabled = Value("disabled")

  /** User is in the process of being deleted. */
  val Deleting = Value("deleting")

  /** User has been already deleted. */
  val Deleted = Value("deleted")

  /** Extractor of user states.
   *
   * @see <a href="http://www.scala-lang.org/old/node/112">Extractor Objects</a>
   */
  def unapply(state: String): Option[UserState] = values.find(_.toString == state.toLowerCase)
}
