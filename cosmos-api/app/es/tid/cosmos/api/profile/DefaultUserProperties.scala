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

package es.tid.cosmos.api.profile

/** A trait that satisfies the default user properties as group, quota, capabilities... . */
trait DefaultUserProperties {

  val defaultGroup: Group = NoGroup
  val defaultQuota: Quota = UnlimitedQuota
  val defaultUserCapabilities: UserCapabilities = UntrustedUserCapabilities
}
