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

/**
 * User identification.
 *
 * @constructor
 * @param realm  Namespace of the identifier
 * @param id     Unique identifier within a realm
 */
case class UserId(realm: String, id: String) {
  require(!realm.trim.isEmpty, "Empty UserId realm")
  require(!id.trim.isEmpty, "Empty UserId id")

  override def toString = s"$id@$realm"
}

object UserId {
  val DefaultRealm: String = "default"

  def apply(id: String): UserId = UserId(DefaultRealm, id)
}