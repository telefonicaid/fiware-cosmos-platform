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
