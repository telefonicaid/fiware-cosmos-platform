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

package es.tid.cosmos.admin.validation

import scalaz._

import es.tid.cosmos.api.profile.dao.GroupDataStore
import es.tid.cosmos.api.quota.GuaranteedGroup

/** Mix-in trait for adding user-group related checks */
trait GroupValidations {
  import Scalaz._

  val store: GroupDataStore

  protected def requireExistingGroup(name: String)
                                    (implicit c: store.Conn): Validation[String, GuaranteedGroup] =
    store.group.lookupByName(name).toSuccess(s"No group with name $name")
}
