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

/** Resource exhausted exception class. This exception is thrown when there are not enough
  * infrastructure resources available to satisfy a request to the infrastructure provider.
  *
  * @param resource the type of resource which was exhausted
  * @param requested the amount of requested resources
  * @param available the amount of available resources
  */
case class ResourceExhaustedException(
    resource: String,
    requested: Int,
    available: Int,
    cause: Throwable = null) extends Exception(
  s"requested $requested resources of type $resource, only $available available",
  cause
)

/** Exception used for when preconditions are not met before attempting to create a number of
  * machines.
  *
  * @param profile the machine profile of the creation request
  * @param numberOfMachines the number of machines that were requested
  * @param reasons the reasons the preconditions were not met
  */
case class PreconditionsNotMetException(
   profile: MachineProfile.Value,
   numberOfMachines: Int,
   reasons: Seq[String]) extends Exception(
  s"""Preconditions not met upon attempting to create $numberOfMachines $profile machines.
      | Causes: ${reasons.mkString(", ")}""".stripMargin
)
