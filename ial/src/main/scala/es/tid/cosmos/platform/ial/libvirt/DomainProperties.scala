package es.tid.cosmos.platform.ial.libvirt

import java.util.UUID

/**
 * A representation of the properties of a libvirt domain.
 */
case class DomainProperties(
    uuid: UUID,
    name: String,
    isActive: Boolean,
    hostname: String,
    ipAddress: String)

