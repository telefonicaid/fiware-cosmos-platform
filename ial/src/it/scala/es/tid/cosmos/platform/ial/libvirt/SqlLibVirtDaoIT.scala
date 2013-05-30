package es.tid.cosmos.platform.ial.libvirt

import org.scalatest.matchers.MustMatchers
import org.scalatest.FlatSpec

class SqlLibVirtDaoIT extends FlatSpec with LibVirtSqlTest with MustMatchers {

  "The libvirt DAO" must "list available servers" in {
    val servers = dao.libVirtServers
    servers must have size (5)
    servers must containServer("andromeda02")
    servers must containServer("andromeda03")
    servers must containServer("andromeda04")
    servers must containServer("andromeda05")
    servers must containServer("andromeda06")
  }
}
