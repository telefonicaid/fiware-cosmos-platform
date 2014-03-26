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

package es.tid.cosmos.infinity.server.authentication.cosmosapi

import scala.util.Success

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import spray.http.HttpEntity

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.authorization.UnixFilePermissions

class ResponseObjectTest extends FlatSpec with MustMatchers {

  "Response object" must "be extracted from HTTP response" in {
    ResponseObject.extractFrom(HttpEntity(
      """{
        | "user": "gandalf",
        | "group": "istari",
        | "accessMask": "752",
        | "origins": "anyHost"
        |}
      """.stripMargin
    )) must be (Success(ResponseObject(
      user = "gandalf",
      group = "istari",
      accessMask = "752",
      origins = None
    )))
  }

  it must "be extracted from HTTP response with missing optional fields" in {
    ResponseObject.extractFrom(HttpEntity(
      """{
        | "user": "gandalf",
        | "group": "istari",
        | "accessMask": "752"
        |}
      """.stripMargin
    )) must be (Success(ResponseObject(
      user = "gandalf",
      group = "istari",
      accessMask = "752",
      origins = None
    )))
  }

  it must "fail to be extracted from HTTP response with missing required fields" in {
    evaluating {
      ResponseObject.extractFrom(HttpEntity(
        """{
          | "user": "gandalf",
          | "origins": "anyHost"
          |}
        """.stripMargin
      )).get
    } must produce [IllegalArgumentException]
  }

  it must "fail to be extracted from HTTP response with invalid access mask value" in {
    evaluating {
      ResponseObject.extractFrom(HttpEntity(
        """{
          | "user": "gandalf",
          | "group": "istari",
          | "accessMask": "This is a invalid access mask value"
          |}
        """.stripMargin
      )).get
    } must produce [IllegalArgumentException]
  }

  it must "fail to be extracted from HTTP response with invalid format" in {
    evaluating {
      ResponseObject.extractFrom(HttpEntity(
        "Hello World! This is a stupid response that does not mean anything!"
      )).get
    } must produce [IllegalArgumentException]
  }

  it must "be converted to user profile with no host access restriction" in {
    ResponseObject(
      user = "gandalf",
      group = "istari",
      accessMask = "755",
      origins = None
    ).toUserProfile must be (UserProfile(
      username = "gandalf",
      group = "istari",
      unixPermissionMask = UnixFilePermissions.fromOctal("755"),
      accessFrom = Set.empty
    ))
  }

  it must "be converted to user profile with host access restriction" in {
    ResponseObject(
      user = "gandalf",
      group = "istari",
      accessMask = "755",
      origins = Some(Seq("192.168.44.86"))
    ).toUserProfile must be (UserProfile(
      username = "gandalf",
      group = "istari",
      unixPermissionMask = UnixFilePermissions.fromOctal("755"),
      accessFrom = Set("192.168.44.86")
    ))
  }
}
