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

package es.tid.cosmos.platform.manager.ial

import java.util.UUID

/**
 * Resource identifier class.
 *
 * @param id the string representation of the identifier
 * @tparam T the type of resource which is being identified
 */
case class Id[+T](id: String) {
  override def toString = id
}

/**
 * Id class companion object.
 */
object Id {
  /**
   * Obtain a new Id object from a random UUID.
   *
   * @tparam T the type of resource the resulting Id is identifying
   * @return the newly created Id
   */
  def apply[T]: Id[T] =  new Id[T](UUID.randomUUID().toString)

  /**
   * Obtain a new Id object from given UUID.
   *
   * @param uuid the UUID used to populate the newly created Id
   * @tparam T the type of resource the resulting Id is identifying
   * @return the newly created Id
   */
  def apply[T](uuid: UUID): Id[T] = new Id[T](uuid.toString)
}

/**
 * An abstraction for the state of any infrastructure resource.
 *
 * @param id the identifier of the resource
 * @param name the (human) name of the resource
 */
sealed abstract class Resource(id: Id[_ <: Resource], name: String)

/**
 * An enumeration of the available machine profiles.
 */
object MachineProfile extends Enumeration {

  type MachineProfile = Value

  /* Uses indices for backwards compability while serializing. */
  val XS = Value(1, "XS machine")
  val S = Value(2, "S machine")
  val M = Value(3, "M machine")
  val X = Value(4, "X machine")
  val XL = Value(5, "XL machine")
}

/**
 * An enumeration of the available machine status.
 */
object MachineStatus extends Enumeration {

  type MachineStatus = Value

  /* Uses indices for backwards compability while serializing. */
  val Provisioning = Value(1, "provisioning")
  val Running = Value(2, "running")
}

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
    ipAddress: String) extends Resource(id, name) {

  override def toString: String =
    s"{ id: '$id', name: '$name', profile: '$profile', status: '$status', " +
      s"hostname: '$hostname', ipAddress: '$ipAddress' }"
}
