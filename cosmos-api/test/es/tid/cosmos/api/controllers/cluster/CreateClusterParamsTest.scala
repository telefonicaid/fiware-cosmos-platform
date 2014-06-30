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
