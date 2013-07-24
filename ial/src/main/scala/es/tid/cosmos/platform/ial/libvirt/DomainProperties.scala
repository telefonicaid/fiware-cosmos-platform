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

import java.util.UUID

/**
 * A representation of the properties of a libvirt domain.
 */
case class DomainProperties(
    uuid: UUID,
    name: String,
    isActive: Boolean,
    profile: MachineProfile.Value,
    hostname: String,
    ipAddress: String)

