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

import es.tid.cosmos.api.profile.dao.GroupDataStore
import es.tid.cosmos.api.quota.GuaranteedGroup

/** Mix-in trait for adding user-group related checks */
trait GroupChecks {
  import Scalaz._

  val store: GroupDataStore

  protected def requireExistingGroup(name: String)
                                    (implicit c: store.Conn): Validation[String, GuaranteedGroup] =
    store.group.lookupByName(name).toSuccess(s"No group with name $name")
}
