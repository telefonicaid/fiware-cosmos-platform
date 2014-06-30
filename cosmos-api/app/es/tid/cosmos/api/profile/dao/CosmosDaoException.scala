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

package es.tid.cosmos.api.profile.dao

import es.tid.cosmos.api.profile.ProfileId

/** Exception to be thrown by implementations of DAOs to provide a clean
  * layer abstraction.
  *
  * @param message Error description
  * @param cause   Deeper cause of the problem
  */
case class CosmosDaoException(message: String, cause: Throwable = null)
  extends RuntimeException(message, cause)

object CosmosDaoException {

  def unknownUser(id: ProfileId) = CosmosDaoException(s"Unknown user with cosmosId=$id")

  def duplicatedHandle(handle: String) =
    CosmosDaoException(s"Cannot use $handle as handle or it will become duplicated")
}
