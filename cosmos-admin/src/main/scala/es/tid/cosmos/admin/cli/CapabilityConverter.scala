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

package es.tid.cosmos.admin.cli

import scala.reflect.runtime.universe._

import org.rogach.scallop.{ArgType, ValueConverter}

import es.tid.cosmos.api.profile.Capability
import es.tid.cosmos.api.profile.Capability.Capability

/** Scallop parser of capabilities. */
private[cli] object CapabilityConverter extends ValueConverter[Capability] {

  override val tag = typeTag[Capability]
  override val argType = ArgType.SINGLE
  private val expectedValuesMessage = s"one of ${Capability.values.mkString(", ")} was expected"

  override def parse(s: List[(String, List[String])]): Either[String, Option[Capability]] =
    s match {
      case (_, text :: Nil) :: Nil => {
        val value = Capability.values.find(_.toString == text)
        if (value.isEmpty) Left(s"$expectedValuesMessage but '$text' found")
        else Right(value)
      }
      case Nil => Right(None)
      case _ => Left(expectedValuesMessage)
    }
}
