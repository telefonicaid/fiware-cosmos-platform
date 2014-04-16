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

package es.tid.cosmos.platform.ial

import com.typesafe.scalalogging.slf4j.Logging

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
) with Logging {
  logger.warn(super.getMessage)
}

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
) with Logging {
  logger.warn(super.getMessage)
}
