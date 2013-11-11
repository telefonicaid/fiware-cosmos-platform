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

package es.tid.cosmos.platform.ial

/**
 * A class representing the state of a machine resource.
 *
 * @param id the identifier of the resource
 * @param name the (human-readable) name of the resource
 * @param profile the profile of the machine
 * @param status the status of the machine
 * @param hostname the hostname of the machine
 * @param ipAddress the IP address for that machine
 */
case class MachineState(
    id: Id[MachineState],
    name: String,
    profile: MachineProfile.Value,
    status: MachineStatus.Value,
    hostname: String,
    ipAddress: String) {

  override def toString: String =
    s"{ id: '$id', name: '$name', profile: '$profile', status: '$status', " +
      s"hostname: '$hostname', ipAddress: '$ipAddress' }"
}
