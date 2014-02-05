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
