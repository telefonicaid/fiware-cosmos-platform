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

package es.tid.cosmos.api

import play.api.data.validation.ValidationError
import play.api.libs.json._
import play.api.libs.functional.syntax._

import es.tid.cosmos.api.auth.ApiCredentials

package object profile {

  /**
   * Represents the Cosmos-specific user profile.
   *
   * @param id             Internal user id
   * @param handle         User login
   * @param quota          Quota for resources
   * @param apiCredentials Credentials for the REST API
   * @param keys           Public keys
   */
  case class CosmosProfile(
      id: Long,
      handle: String,
      quota: Quota,
      apiCredentials: ApiCredentials,
      keys: Seq[NamedKey]) {

    require(HandleConstraint(handle), s"Invalid handle: $handle")
    require(duplicatedKeys.isEmpty, s"Duplicated keys: ${duplicatedKeys.mkString("", ", ", "")}")

    private def duplicatedKeys = for {
      (name, group) <- keys.groupBy(_.name) if group.size > 1
    } yield name
  }

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

  /**
   * Defines the resource quota, which can be empty, unlimited or finite.
   */
  sealed abstract class Quota {
    def withinQuota(request: Int): Boolean
  }

  case object EmptyQuota extends Quota {
    def withinQuota(request: Int): Boolean = false
  }
  case object UnlimitedQuota extends Quota {
    def withinQuota(request: Int): Boolean = true
  }
  case class FiniteQuota(limit: Int) extends Quota {
    require(limit > 0, s"Invalid quota: $limit")
    def withinQuota(request: Int): Boolean = request <= limit
  }

  object Quota {

    /**
     * Determines what type of quota is applicable for a given input value.
     *
     * @param limit  Can be empty, or if not, the numerical limit of the quota.
     * @return       The type of quota determined by the input limit.
     */
    def apply(limit: Option[Int]): Quota = limit match {
      case None => UnlimitedQuota
      case Some(value) => if (value == 0) EmptyQuota else FiniteQuota(value)
    }
  }
}
