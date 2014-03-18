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

import scalaz._

import es.tid.cosmos.admin.Util
import es.tid.cosmos.api.profile.dao.GroupDataStore
import es.tid.cosmos.api.quota.GuaranteedGroup

/** Mix-in trait for adding user-group related checks */
trait GroupChecks {
  import Scalaz._

  val store: GroupDataStore

  /** ''With side-effects.''
    * Attempt to load the group by name and print an error message otherwise.
    *
    * @param name the group's name
    * @param c the DAO connection
    * @return the group iff found `None` otherwise
    */
  protected def withGroup(name: String)(implicit c: store.Conn): Option[GuaranteedGroup] =
    Util.whenEmpty(store.group.lookupByName(name)) {
      println(s"No group with name $name")
    }

  protected def requireExistingGroup(name: String)
                                    (implicit c: store.Conn): Validation[String, GuaranteedGroup] =
    store.group.lookupByName(name).toSuccess(s"No group with name $name")
}
