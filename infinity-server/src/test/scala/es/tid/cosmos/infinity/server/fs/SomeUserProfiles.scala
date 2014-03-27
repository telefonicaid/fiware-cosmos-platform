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

package es.tid.cosmos.infinity.server.fs

import es.tid.cosmos.infinity.server.authentication.UserProfile

trait SomeUserProfiles {

  val bob   = UserProfile("bob", "staff")
  val alice = UserProfile("alice", "staff")
  val john  = UserProfile("john", "other")
  val hdfs  = UserProfile("hdfs", "hdfs", superuser = true)

}
