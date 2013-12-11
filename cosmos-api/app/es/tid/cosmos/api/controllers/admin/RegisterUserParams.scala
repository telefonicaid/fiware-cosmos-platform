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

import play.api.data.validation.ValidationError
import play.api.libs.functional.syntax._
import play.api.libs.json._

import es.tid.cosmos.api.profile.{EmailConstraint, HandleConstraint, AuthorizedKeyConstraint}

/** Parameters for automated user registration. */
case class RegisterUserParams(
  authId: String,
  authRealm: String,
  handle: Option[String],
  email: String,
  sshPublicKey: String)

object RegisterUserParams {

  implicit val registerUserParamsReads: Reads[RegisterUserParams] = (
    (__ \ "authId").read[String]
      .filter(ValidationError("empty authId"))(_.length > 0) ~
    (__ \ "authRealm").read[String] ~
    (__ \ "handle").readNullable[String]
      .filter(ValidationError("not a unix handle"))(validateHandle) ~
    (__ \ "email").read[String]
      .filter(ValidationError("not a valid email"))(EmailConstraint.apply) ~
    (__ \ "sshPublicKey").read[String]
      .filter(ValidationError("not a valid public key"))(AuthorizedKeyConstraint.apply)
  )(RegisterUserParams.apply _)

  private def validateHandle(maybeHandle: Option[String]) =
    maybeHandle.map(HandleConstraint.apply).getOrElse(true)
}
