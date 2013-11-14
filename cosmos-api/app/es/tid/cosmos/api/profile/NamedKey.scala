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

package es.tid.cosmos.api.profile

import play.api.libs.json._
import play.api.libs.functional.syntax._
import play.api.data.validation.ValidationError

/**
 * A public key annotated with a user-defined name.
 *
 * @param name       Key name
 * @param signature  Public key signature
 */
case class NamedKey(name: String, signature: String) {
  require(!name.isEmpty, s"Empty name")
  require(AuthorizedKeyConstraint(signature), s"Not a valid signature: $signature")
}

object NamedKey {

  implicit val namedKeyReads: Reads[NamedKey] = (
    (__ \ "name").read[String].filter(ValidationError("empty name"))(_.length > 0) ~
    (__ \ "signature").read[String]
      .filter(ValidationError("not an ssh key"))(AuthorizedKeyConstraint.apply)
  )(NamedKey.apply _)

  implicit object NamedKeyWrites extends Writes[NamedKey] {
    def writes(k: NamedKey): JsValue = Json.obj(
      "name" -> k.name,
      "signature" -> k.signature
    )
  }
}
