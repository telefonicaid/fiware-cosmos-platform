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

package es.tid.cosmos.api.controllers.cluster

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.{JsSuccess, Json}

import es.tid.cosmos.servicemanager.ClusterName

class CreateClusterParamsTest extends FlatSpec with MustMatchers {

  val params = CreateClusterParams(
    name = ClusterName("new cluster"),
    size = 150,
    optionalServices = Seq("HIVE"),
    shared = true
  )
  val json = Json.obj(
    "name" -> "new cluster",
    "size" -> 150,
    "optionalServices" -> Json.arr("HIVE"),
    "shared" -> true
  )

  "Create cluster params" must "be parsed from JSON" in {
    Json.fromJson[CreateClusterParams](json) must be (JsSuccess(params))
  }

  it must "not be parsed when cluster name is too long" in {
    val tooLongName = Json.obj(
      "name" -> Seq.fill(ClusterName.MaxLength + 1)("X").mkString
    )
    val result = Json.fromJson[CreateClusterParams](json ++ tooLongName)
    result.asOpt must be ('empty)
    result.toString must include ("cluster name is too long")
  }
}
