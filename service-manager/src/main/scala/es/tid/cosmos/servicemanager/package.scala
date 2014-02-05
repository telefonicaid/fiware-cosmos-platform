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

package es.tid.cosmos

import es.tid.cosmos.common.{ExecutableValidation, PassThrough}
import es.tid.cosmos.servicemanager.clusters.ClusterId

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
}
