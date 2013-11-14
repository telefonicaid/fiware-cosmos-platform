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

import es.tid.cosmos.platform.ial.MachineProfile

/**
 * A transient implementation for a libvirt server properties.
 */
class TransientLibVirtServerProperties(
    val name: String,
    val description: String,
    val profile: MachineProfile.Value,
    val rack: String,
    val connectionChain: String,
    val domainHostname: String,
    val domainIpAddress: String) extends LibVirtServerProperties
