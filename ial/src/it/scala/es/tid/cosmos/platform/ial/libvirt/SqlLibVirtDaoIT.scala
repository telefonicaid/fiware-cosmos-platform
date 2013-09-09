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

package es.tid.cosmos.platform.ial.libvirt

import org.scalatest.matchers.MustMatchers
import org.scalatest.FlatSpec

import es.tid.cosmos.platform.common.scalatest.tags.HasExternalDependencies

class SqlLibVirtDaoIT extends FlatSpec with LibVirtSqlTest with MustMatchers {

  "The libvirt DAO" must "list available servers" taggedAs HasExternalDependencies in {
    val servers = dao.libVirtServers
    servers must have size (5)
    servers must containServer("andromeda02")
    servers must containServer("andromeda03")
    servers must containServer("andromeda04")
    servers must containServer("andromeda05")
    servers must containServer("andromeda06")
  }
}
