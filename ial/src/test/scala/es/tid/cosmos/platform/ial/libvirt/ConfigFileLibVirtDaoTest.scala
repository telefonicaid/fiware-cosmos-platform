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

package es.tid.cosmos.platform.ial.libvirt

import com.typesafe.config.ConfigFactory
import org.scalatest.{Inside, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.ial.MachineProfile

class ConfigFileLibVirtDaoTest extends FlatSpec with MustMatchers with Inside {

  "A config file libvirt DAO" must "read config from file" in {
    val conf = ConfigFactory.load("normal")
    val dao = new ConfigFileLibVirtDao(conf)

    inside(dao.libVirtServers) {
      case Seq(srv1: LibVirtServerProperties, srv2: LibVirtServerProperties) =>
        srv1.name must be ("andromeda02")
        srv1.description must be ("Andromeda 02")
        srv1.connectionChain must be (
          "openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock"
        )
        srv1.domainHostname must be ("andromeda-compute02")
        srv1.domainIpAddress must be ("192.168.63.32")
        srv1.profile must be (MachineProfile.G1Compute)
        srv1.rack must be ("rack02")

        srv2.name must be ("andromeda03")
        srv2.description must be ("Andromeda 03")
        srv2.connectionChain must be (
          "openvz+ssh://cosmos@192.168.63.13/system?socket=/var/run/libvirt/libvirt-sock"
        )
        srv2.domainHostname must be ("andromeda-compute03")
        srv2.domainIpAddress must be ("192.168.63.33")
        srv2.profile must be (MachineProfile.G1Compute)
        srv2.rack must be ("rack01")
    }
  }

  it must "ignore servers with enabled = false option" in {
    val conf = ConfigFactory.load("enabled-false")
    val dao = new ConfigFileLibVirtDao(conf)

    inside(dao.libVirtServers) {
      case Seq(srv: LibVirtServerProperties) =>
        srv.name must be ("andromeda02")
    }
  }

  it must "accept servers with missing enabled option" in {
    val conf = ConfigFactory.load("enabled-missing")
    val dao = new ConfigFileLibVirtDao(conf)

    inside(dao.libVirtServers) {
      case Seq(srv: LibVirtServerProperties, _) =>
        srv.name must be ("andromeda02")
    }
  }

  it must "default description from name when missing" in {
    val conf = ConfigFactory.load("description-missing")
    val dao = new ConfigFileLibVirtDao(conf)

    inside(dao.libVirtServers) {
      case Seq(srv: LibVirtServerProperties, _) =>
        srv.description must be ("andromeda02")
    }
  }
}
