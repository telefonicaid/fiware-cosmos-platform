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
