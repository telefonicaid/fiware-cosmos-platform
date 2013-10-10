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

package es.tid.cosmos.api.controllers.admin

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.libs.json.{JsObject, JsSuccess, Json}

class RegisterUserParamsTest extends FlatSpec with MustMatchers {

  val validJson = Json.obj(
    "authId" -> "id",
    "authRealm" -> "realm",
    "handle" -> "handle",
    "sshPublicKey" -> "ssh-rsa ABCDEFG handle@realm"
  )
  val validParams = RegisterUserParams(
    authId = "id",
    authRealm = "realm",
    handle = Some("handle"),
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
}
