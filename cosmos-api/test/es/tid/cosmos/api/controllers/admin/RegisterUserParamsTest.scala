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

package es.tid.cosmos.api.controllers.admin

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.{JsObject, JsSuccess, Json}

class RegisterUserParamsTest extends FlatSpec with MustMatchers {

  val validJson = Json.obj(
    "authId" -> "id",
    "authRealm" -> "realm",
    "email" -> "handle@realm",
    "handle" -> "handle",
    "sshPublicKey" -> "ssh-rsa ABCDEFG handle@realm"
  )
  val validParams = RegisterUserParams(
    authId = "id",
    authRealm = "realm",
    handle = Some("handle"),
    email = "handle@realm",
    sshPublicKey = "ssh-rsa ABCDEFG handle@realm"
  )

  def parse(json: JsObject) = Json.fromJson[RegisterUserParams](json)

  "User registration params" must "be readable from JSON" in {
    parse(validJson) must be (JsSuccess(validParams))
  }

  it must "allow missing handle in JSON" in {
    parse(validJson - "handle") must be (JsSuccess(validParams.copy(handle = None)))
  }

  it must "reject empty auth ids when reading from JSON" in {
    val result = parse(validJson ++ Json.obj("authId" -> ""))
    result must not be JsSuccess
    result.toString must include ("empty authId")
  }

  it must "reject invalid handles when reading from JSON" in {
    val result = parse(validJson ++ Json.obj("handle" -> "1nvaa,|li d"))
    result must not be JsSuccess
    result.toString must include ("not a unix handle")
  }

  it must "reject invalid SSH keys when reading from JSON" in {
    val result = parse(validJson ++ Json.obj("sshPublicKey" -> "not and ssh key"))
    result must not be JsSuccess
    result.toString must include ("not a valid public key")
  }

  it must "reject invalid emails" in {
    val result = parse(validJson ++ Json.obj("email" -> "not an email"))
    result must not be JsSuccess
    result.toString must include ("not a valid email")
  }
}
