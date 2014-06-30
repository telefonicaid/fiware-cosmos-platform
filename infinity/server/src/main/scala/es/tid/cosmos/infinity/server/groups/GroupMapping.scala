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

package es.tid.cosmos.infinity.server.groups

private[groups] trait GroupMapping {

  /** Groups of an user.
    *
    * The list must have at least one group for existing users and be empty for unknown ones.
    */
  type GroupList = Seq[String]

  /** Retrieve groups for a given user.
    *
    * @return None if the user doesn't exist or the list of groups otherwise
    */
  def groupsFor(handle: String): GroupList
}
