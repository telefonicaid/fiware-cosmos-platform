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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.Capability

class CapabilityConverterTest extends FlatSpec with MustMatchers {

  "A capability converter" must "parse a valid capability as a single argument" in {
    val singleArgument = List("capability" -> List("is_operator"))
    CapabilityConverter.parse(singleArgument) must be (Right(Some(Capability.IsOperator)))
  }

  it must "reject an unknown capability with the list of capabilities" in {
    val singleArgument = List("capability" -> List("is_unknown"))
    CapabilityConverter.parse(singleArgument) must be (Left(
      s"one of ${Capability.values.mkString(", ")} was expected but 'is_unknown' found"))
  }
  
  it must "reject multiple arguments passed as a capability" in {
    val multipleArguments = List("capability" -> List("is", "operator"))
    CapabilityConverter.parse(multipleArguments) must be ('left)
  }
}
