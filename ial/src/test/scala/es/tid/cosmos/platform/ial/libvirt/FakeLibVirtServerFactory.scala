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

/**
 * A libvirt server factory which returns the same server for each server name.
 */
class FakeLibVirtServerFactory extends (LibVirtServerProperties => LibVirtServer) {

  private val servers = scala.collection.mutable.Map[String, LibVirtServer]()

  def apply(properties: LibVirtServerProperties): LibVirtServer =
    servers.get(properties.name).getOrElse {
      val srv = new FakeLibVirtServer(properties)
      servers.put(properties.name, srv)
      srv
    }
}
