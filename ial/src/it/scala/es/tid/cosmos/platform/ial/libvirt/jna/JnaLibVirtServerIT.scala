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

package es.tid.cosmos.platform.ial.libvirt.jna

import scala.concurrent.Await
import scala.concurrent.duration.Duration
import scala.concurrent.ExecutionContext.Implicits.global

import org.scalatest.{BeforeAndAfter, FlatSpec}
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.tags.HasExternalDependencies
import es.tid.cosmos.platform.ial.libvirt.TransientLibVirtServerProperties
import es.tid.cosmos.platform.ial.MachineProfile

class JnaLibVirtServerIT extends FlatSpec with MustMatchers with BeforeAndAfter {

  var server: JnaLibVirtServer = null

  before {
    server = new JnaLibVirtServer(new TransientLibVirtServerProperties(
        name = "Test",
        description = "Test libvirt Server",
        MachineProfile.G1Compute,
        rack = "rack01",
        connectionChain =
          "openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock",
        domainHostname = "andromeda52",
        domainIpAddress = "192.168.63.62"))
    Await.ready(server.destroyDomain(), Duration.Inf)
  }

  "The JNA libvirt server" must "create a new domain" taggedAs HasExternalDependencies in {
    val dom_> = server.createDomain()
    val dom = Await.result(dom_>, Duration.Inf)
    dom.name must be (server.domainName)
    dom must be ('active)
  }

  it must "indicate the domain is created after creation" taggedAs HasExternalDependencies in {
    val dom_> = server.createDomain()
    Await.ready(dom_>, Duration.Inf)
    Await.result(server.isCreated(), Duration.Inf) must be (true)
  }

  it must "indicate the domain is not created before creation" taggedAs HasExternalDependencies in {
    Await.result(server.isCreated(), Duration.Inf) must be (false)
  }

  it must "destroy existing domain" taggedAs HasExternalDependencies in {
    val dest_> = server.destroyDomain()
    Await.result(dest_>, Duration.Inf)
  }

  it must "create the domain even if it already exists" taggedAs HasExternalDependencies in {
    val dom_> = server.createDomain().flatMap(_ => server.createDomain())
    val dom = Await.result(dom_>, Duration.Inf)
    dom.name must be (server.domainName)
    dom must be ('active)
  }

  after { Await.ready(server.destroyDomain(), Duration.Inf) }
}
