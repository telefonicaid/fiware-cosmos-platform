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

package es.tid.cosmos.servicemanager.ambari.rest.as

import net.liftweb.json.JsonAST.{JValue, JString}

/** Helper object to extract values from JSON lists. */
object FlatValues {

  /** Extract the list of child values of the attributes of a given parent list within a JSON value.
   *
   * @param json the json from which to extract the values
   * @param parentListName the name of the parent list
   * @param child the name of the child attribute found within each list entry
   * @return the values of the children attributes
   */
  def apply(json: JValue, parentListName: String, child: String): Seq[String] = for {
    parentList <- (json \\ parentListName).children
    JString(serviceName) <- parentList \\ child
  } yield serviceName
}
