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

package es.tid.cosmos.admin.validation

import es.tid.cosmos.api.profile.{Group, CosmosProfileDao}
import es.tid.cosmos.admin.Util

/** Mix-in trait for adding user-group related checks */
trait GroupChecks {

  val dao: CosmosProfileDao

  /** ''With side-effects.''
    * Attempt to load the group by name and print an error message otherwise.
    *
    * @param name the group's name
    * @param c the DAO connection
    * @return the group iff found `None` otherwise
    */
  protected def withGroup(name: String)
                         (implicit c: this.dao.type#Conn): Option[Group] =
    Util.whenEmpty(dao.getGroups.find(_.name == name)) {
      println(s"No group with name $name")
    }
}
