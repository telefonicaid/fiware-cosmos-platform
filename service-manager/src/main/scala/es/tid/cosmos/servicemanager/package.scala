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

package es.tid.cosmos

import es.tid.cosmos.common.{ExecutableValidation, PassThrough}
import es.tid.cosmos.servicemanager.clusters.ClusterId
import es.tid.cosmos.servicemanager.services.Service

/** Service Manager related type aliases. */
package object servicemanager {

  /** Type alias for executable validations that are specific for a cluster ID.
    * This allows to further restrict a given validation to a specific cluster ID.
    * It offers a 2-step evaluation whereby the 1st step is to restrict it to a specific
    * cluster ID and the 2nd to actually execute the validation.
    *
    * e.g.
    *
    * {{{
    * val validation = (clusterId) => () => if (clusterId == "myId") x.successNel
    *   else "Not myID".failureNel
    * val executable = validation(clusterId("myId"))
    * executable().isSuccess == true
    * }}}
    *
    * @see [[es.tid.cosmos.common.ExecutableValidation]]
    */
  type ClusterExecutableValidation = ClusterId => ExecutableValidation

  /** ''Null'' object that does a pass-through without any validation */
  val UnfilteredPassThrough: ClusterExecutableValidation = (_) => PassThrough

  type AnyServiceInstance = ServiceInstance[_ <: Service]
}
